//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2023 Julian Nechaevsky
// Copyright(C) 2020-2025 Leonid Murin (Dasperal)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//    Reading of MIDI files.
//


#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "doomtype.h"
#include "m_misc.h"
#include "i_system.h"
#include "i_swap.h"
#include "midifile.h"
#include "jn.h"

#define HEADER_CHUNK_ID "MThd"
#define TRACK_CHUNK_ID  "MTrk"
#define MAX_BUFFER_SIZE 0x10000

// haleyjd 09/09/10: packing required
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef PACKED_STRUCT (
{
    byte chunk_id[4];
    unsigned int chunk_size;
}) chunk_header_t;

typedef PACKED_STRUCT (
{
    chunk_header_t chunk_header;
    unsigned short format_type;
    unsigned short num_tracks;
    unsigned short time_division;
}) midi_header_t;

// haleyjd 09/09/10: packing off.
#ifdef _MSC_VER
#pragma pack(pop)
#endif

typedef struct
{
    // Length in bytes:

    unsigned int data_len;

    // Events in this track:

    midi_event_t *events;
    int num_events;
} midi_track_t;

struct midi_track_iter_s
{
    midi_track_t *track;
    unsigned int position;
};

struct midi_file_s
{
    midi_header_t header;

    // All tracks in this file:
    midi_track_t *tracks;
    unsigned int num_tracks;

    // Data buffer used to store data read for SysEx or meta events:
    byte *buffer;
    unsigned int buffer_size;
};

// Check the header of a chunk:

static boolean CheckChunkHeader(chunk_header_t *chunk,
                                char *expected_id)
{
    boolean result;
    
    result = (memcmp((char *) chunk->chunk_id, expected_id, 4) == 0);

    if (!result)
    {
        printf(english_language ?
                        "CheckChunkHeader: Expected '%s' chunk header, got '%c%c%c%c'\n" :
                        "CheckChunkHeader: ожидаемый заголовок: '%s', получен: '%c%c%c%c'\n",
                        expected_id,
                        chunk->chunk_id[0], chunk->chunk_id[1],
                        chunk->chunk_id[2], chunk->chunk_id[3]);
    }

    return result;
}

// Read a single byte.  Returns false on error.

static boolean ReadByte(byte *result, FILE *stream)
{
    int c;

    c = fgetc(stream);

    if (c == EOF)
    {
        printf(english_language ?
                "ReadByte: Unexpected end of file\n" :
                "ReadByte: неожиданный конец файла\n");
        return false;
    }
    else
    {
        *result = (byte) c;

        return true;
    }
}

// Read a variable-length value.

static boolean ReadVariableLength(unsigned int *result, FILE *stream)
{
    int i;
    byte b = 0;

    *result = 0;

    for (i=0; i<4; ++i)
    {
        if (!ReadByte(&b, stream))
        {
            printf(english_language ?
                    "ReadVariableLength: Error while reading variable-length value\n" :
                    "ReadVariableLength: ошибка при чтении переменной длины\n");
            return false;
        }

        // Insert the bottom seven bits from this byte.

        *result <<= 7;
        *result |= b & 0x7f;

        // If the top bit is not set, this is the end.

        if ((b & 0x80) == 0)
        {
            return true;
        }
    }

    printf(english_language ?
            "ReadVariableLength: Variable-length value too long: maximum of four bytes\n" :
            "ReadVariableLength: превышел лимит переменной длины: максимум 4 байта\n");
    return false;
}

// Read a byte sequence into the data buffer.

static void *ReadByteSequence(unsigned int num_bytes, FILE *stream)
{
    unsigned int i;
    byte *result;

    // Allocate a buffer. Allocate one extra byte, as malloc(0) is
    // non-portable.

    result = malloc(num_bytes + 1);

    if (result == NULL)
    {
        printf(english_language ?
                "ReadByteSequence: Failed to allocate buffer\n" :
                "ReadByteSequence: ошибка обнарушения буфера\n");
        return NULL;
    }

    // Read the data:

    for (i=0; i<num_bytes; ++i)
    {
        if (!ReadByte(&result[i], stream))
        {
            printf(english_language ?
                            "ReadByteSequence: Error while reading byte %u\n" :
                            "ReadByteSequence: ошибка чтения байта %u\n",
                            i);
            free(result);
            return NULL;
        }
    }

    return result;
}

// Read a MIDI channel event.
// two_param indicates that the event type takes two parameters
// (three byte) otherwise it is single parameter (two byte)

static boolean ReadChannelEvent(midi_event_t *event,
                                byte event_type, boolean two_param,
                                FILE *stream)
{
    byte b = 0;

    // Set basics:

    event->event_type = event_type & 0xf0;
    event->data.channel.channel = event_type & 0x0f;

    // Read parameters:

    if (!ReadByte(&b, stream))
    {
        printf(english_language ?
                "ReadChannelEvent: Error while reading channel event parameters\n" :
                "ReadChannelEvent: ошибка чтения параметров событий канала\n");
        return false;
    }

    event->data.channel.param1 = b;

    // Second parameter:

    if (two_param)
    {
        if (!ReadByte(&b, stream))
        {
            printf(english_language ?
                    "ReadChannelEvent: Error while reading channel event parameters\n" :
                    "ReadChannelEvent: ошибка чтения параметров событий канала\n");
            return false;
        }

        event->data.channel.param2 = b;
    }

    return true;
}

// Read sysex event:

static boolean ReadSysExEvent(midi_event_t *event, int event_type,
                              FILE *stream)
{
    event->event_type = event_type;

    if (!ReadVariableLength(&event->data.sysex.length, stream))
    {
        printf(english_language ?
                "ReadSysExEvent: Failed to read length of SysEx block\n" :
                "ReadSysExEvent: ошибка чтения длины блока SysEx\n");
        return false;
    }

    // Read the byte sequence:

    event->data.sysex.data = ReadByteSequence(event->data.sysex.length, stream);

    if (event->data.sysex.data == NULL)
    {
        printf(english_language ?
                "ReadSysExEvent: Failed while reading SysEx event\n" :
                "ReadSysExEvent: ошибка чтения события SysEx\n");
        return false;
    }

    return true;
}

// Read meta event:

static boolean ReadMetaEvent(midi_event_t *event, FILE *stream)
{
    byte b = 0;

    event->event_type = MIDI_EVENT_META;

    // Read meta event type:

    if (!ReadByte(&b, stream))
    {
        printf(english_language ?
                "ReadMetaEvent: Failed to read meta event type\n" :
                "ReadMetaEvent: ошибка чтения типа мета-события\n");
        return false;
    }

    event->data.meta.type = b;

    // Read length of meta event data:

    if (!ReadVariableLength(&event->data.meta.length, stream))
    {
        printf(english_language ?
                "ReadSysExEvent: Failed to read length of SysEx block\n" :
                "ReadSysExEvent: ошибка чтения длины блока SysEx\n");
        return false;
    }

    // Read the byte sequence:

    event->data.meta.data = ReadByteSequence(event->data.meta.length, stream);

    if (event->data.meta.data == NULL)
    {
        printf(english_language ?
                "ReadSysExEvent: Failed while reading SysEx event\n" :
                "ReadSysExEvent: ошибка чтения события SysEx\n");
        return false;
    }

    return true;
}

static boolean ReadEvent(midi_event_t *event, unsigned int *last_event_type,
                         FILE *stream)
{
    byte event_type = 0;

    if (!ReadVariableLength(&event->delta_time, stream))
    {
        printf(english_language ?
                "ReadEvent: Failed to read event timestamp\n" :
                "ReadEvent: ошибка чтения временного штампа события\n");
        return false;
    }

    if (!ReadByte(&event_type, stream))
    {
        printf(english_language ?
                "ReadEvent: Failed to read event type\n" :
                "ReadEvent: ошибка чтения типа события\n");
        return false;
    }

    // All event types have their top bit set.  Therefore, if 
    // the top bit is not set, it is because we are using the "same
    // as previous event type" shortcut to save a byte.  Skip back
    // a byte so that we read this byte again.

    if ((event_type & 0x80) == 0)
    {
        event_type = *last_event_type;

        if (fseek(stream, -1, SEEK_CUR) < 0)
        {
            printf(english_language ?
                    "ReadEvent: Unable to seek in stream\n" :
                    "ReadEvent: ошибка обнаружения временного штампа\n");
            return false;
        }
    }
    else
    {
        *last_event_type = event_type;
    }

    // Check event type:

    switch (event_type & 0xf0)
    {
        // Two parameter channel events:

        case MIDI_EVENT_NOTE_OFF:
        case MIDI_EVENT_NOTE_ON:
        case MIDI_EVENT_AFTERTOUCH:
        case MIDI_EVENT_CONTROLLER:
        case MIDI_EVENT_PITCH_BEND:
            return ReadChannelEvent(event, event_type, true, stream);

        // Single parameter channel events:

        case MIDI_EVENT_PROGRAM_CHANGE:
        case MIDI_EVENT_CHAN_AFTERTOUCH:
            return ReadChannelEvent(event, event_type, false, stream);

        default:
            break;
    }

    // Specific value?

    switch (event_type)
    {
        case MIDI_EVENT_SYSEX:
        case MIDI_EVENT_SYSEX_SPLIT:
            return ReadSysExEvent(event, event_type, stream);

        case MIDI_EVENT_META:
            return ReadMetaEvent(event, stream);

        default:
            break;
    }

    printf(english_language ?
            "ReadEvent: Unknown MIDI event type: 0x%x\n" :
            "ReadEvent: неизвестный тип события MIDI: 0x%x\n",
            event_type);
    return false;
}

// Free an event:

static void FreeEvent(midi_event_t *event)
{
    // Some event types have dynamically allocated buffers assigned
    // to them that must be freed.

    switch (event->event_type)
    {
        case MIDI_EVENT_SYSEX:
        case MIDI_EVENT_SYSEX_SPLIT:
            free(event->data.sysex.data);
            break;

        case MIDI_EVENT_META:
            free(event->data.meta.data);
            break;

        default:
            // Nothing to do.
            break;
    }
}

// Read and check the track chunk header

static boolean ReadTrackHeader(midi_track_t *track, FILE *stream)
{
    size_t records_read;
    chunk_header_t chunk_header;

    records_read = fread(&chunk_header, sizeof(chunk_header_t), 1, stream);

    if (records_read < 1)
    {
        return false;
    }

    if (!CheckChunkHeader(&chunk_header, TRACK_CHUNK_ID))
    {
        return false;
    }

    track->data_len = SDL_SwapBE32(chunk_header.chunk_size);

    return true;
}

static boolean ReadTrack(midi_track_t *track, FILE *stream)
{
    midi_event_t *new_events;
    midi_event_t *event;
    unsigned int last_event_type;

    track->num_events = 0;
    track->events = NULL;

    // Read the header:

    if (!ReadTrackHeader(track, stream))
    {
        return false;
    }

    // Then the events:

    last_event_type = 0;

    for (;;)
    {
        // Resize the track slightly larger to hold another event:

        new_events = I_Realloc(track->events, 
                             sizeof(midi_event_t) * (track->num_events + 1));

        if (new_events == NULL)
        {
            return false;
        }

        track->events = new_events;

        // Read the next event:

        event = &track->events[track->num_events];
        if (!ReadEvent(event, &last_event_type, stream))
        {
            return false;
        }

        ++track->num_events;

        // End of track?

        if (event->event_type == MIDI_EVENT_META
         && event->data.meta.type == MIDI_META_END_OF_TRACK)
        {
            break;
        }
    }

    return true;
}

// Free a track:

static void FreeTrack(midi_track_t *track)
{
    unsigned int i;

    for (i=0; i<track->num_events; ++i)
    {
        FreeEvent(&track->events[i]);
    }

    free(track->events);
}

static boolean ReadAllTracks(midi_file_t *file, FILE *stream)
{
    unsigned int i;

    // Allocate list of tracks and read each track:

    file->tracks = malloc(sizeof(midi_track_t) * file->num_tracks);

    if (file->tracks == NULL)
    {
        return false;
    }

    memset(file->tracks, 0, sizeof(midi_track_t) * file->num_tracks);

    // Read each track:

    for (i=0; i<file->num_tracks; ++i)
    {
        if (!ReadTrack(&file->tracks[i], stream))
        {
            return false;
        }
    }

    return true;
}

// Read and check the header chunk.

static boolean ReadFileHeader(midi_file_t *file, FILE *stream)
{
    size_t records_read;
    unsigned int format_type;

    records_read = fread(&file->header, sizeof(midi_header_t), 1, stream);

    if (records_read < 1)
    {
        return false;
    }

    if (!CheckChunkHeader(&file->header.chunk_header, HEADER_CHUNK_ID)
     || SDL_SwapBE32(file->header.chunk_header.chunk_size) != 6)
    {
        printf(english_language ?
                "ReadFileHeader: Invalid MIDI chunk header! chunk_size=%i\n" :
                "ReadFileHeader: некорректный заголовок MIDI! chunk_size=%i\n",
                        SDL_SwapBE32(file->header.chunk_header.chunk_size));
        return false;
    }

    format_type = SDL_SwapBE16(file->header.format_type);
    file->num_tracks = SDL_SwapBE16(file->header.num_tracks);

    if ((format_type != 0 && format_type != 1)
     || file->num_tracks < 1)
    {
        printf(english_language ?
                "ReadFileHeader: Only type 0/1 MIDI files supported!\n" :
                "ReadFileHeader: поддерживаются только 0/1 типы MIDI-файлов!\n");
        return false;
    }

    return true;
}

void MIDI_FreeFile(midi_file_t *file)
{
    int i;

    if (file->tracks != NULL)
    {
        for (i=0; i<file->num_tracks; ++i)
        {
            FreeTrack(&file->tracks[i]);
        }

        free(file->tracks);
    }

    free(file);
}

midi_file_t *MIDI_LoadFile(char *filename)
{
    midi_file_t *file;
    FILE *stream;

    file = malloc(sizeof(midi_file_t));

    if (file == NULL)
    {
        return NULL;
    }

    file->tracks = NULL;
    file->num_tracks = 0;
    file->buffer = NULL;
    file->buffer_size = 0;

    // Open file

    stream = M_fopen(filename, "rb");

    if (stream == NULL)
    {
        printf(english_language ?
                "MIDI_LoadFile: Failed to open '%s'\n" :
                "MIDI_LoadFile: ошибка открытия '%s'\n",
                filename);
        MIDI_FreeFile(file);
        return NULL;
    }

    // Read MIDI file header

    if (!ReadFileHeader(file, stream))
    {
        fclose(stream);
        MIDI_FreeFile(file);
        return NULL;
    }

    // Read all tracks:

    if (!ReadAllTracks(file, stream))
    {
        fclose(stream);
        MIDI_FreeFile(file);
        return NULL;
    }

    fclose(stream);

    return file;
}

// Get the number of tracks in a MIDI file.

unsigned int MIDI_NumTracks(midi_file_t *file)
{
    return file->num_tracks;
}

// Get the number of events in a MIDI file.

unsigned int MIDI_NumEvents(midi_file_t *file)
{
    int i;
    unsigned int num_events = 0;

    for (i = 0; i < file->num_tracks; ++i)
    {
        num_events += file->tracks[i].num_events;
    }

    return num_events;
}

// Start iterating over the events in a track.

midi_track_iter_t *MIDI_IterateTrack(midi_file_t *file, unsigned int track)
{
    midi_track_iter_t *iter;

    assert(track < file->num_tracks);

    iter = malloc(sizeof(*iter));
    iter->track = &file->tracks[track];
    iter->position = 0;

    return iter;
}

void MIDI_FreeIterator(midi_track_iter_t *iter)
{
    free(iter);
}

// Get the time until the next MIDI event in a track.

unsigned int MIDI_GetDeltaTime(midi_track_iter_t *iter)
{
    if (iter->position < iter->track->num_events)
    {
        midi_event_t *next_event;

        next_event = &iter->track->events[iter->position];

        return next_event->delta_time;
    }
    else
    {
        return 0;
    }
}

// Get a pointer to the next MIDI event.

int MIDI_GetNextEvent(midi_track_iter_t *iter, midi_event_t **event)
{
    if (iter->position < iter->track->num_events)
    {
        *event = &iter->track->events[iter->position];
        ++iter->position;

        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int MIDI_GetFileTimeDivision(midi_file_t *file)
{
    short result = SDL_SwapBE16(file->header.time_division);

    // Negative time division indicates SMPTE time and must be handled
    // differently.
    if (result < 0)
    {
        return (signed int)(-(result/256))
             * (signed int)(result & 0xFF);
    }
    else
    {
        return result;
    }
}

void MIDI_RestartIterator(midi_track_iter_t *iter)
{
    iter->position = 0;
}

#ifdef TEST

static char *MIDI_EventTypeToString(midi_event_type_t event_type)
{
    switch (event_type)
    {
        case MIDI_EVENT_NOTE_OFF:
            return "MIDI_EVENT_NOTE_OFF";
        case MIDI_EVENT_NOTE_ON:
            return "MIDI_EVENT_NOTE_ON";
        case MIDI_EVENT_AFTERTOUCH:
            return "MIDI_EVENT_AFTERTOUCH";
        case MIDI_EVENT_CONTROLLER:
            return "MIDI_EVENT_CONTROLLER";
        case MIDI_EVENT_PROGRAM_CHANGE:
            return "MIDI_EVENT_PROGRAM_CHANGE";
        case MIDI_EVENT_CHAN_AFTERTOUCH:
            return "MIDI_EVENT_CHAN_AFTERTOUCH";
        case MIDI_EVENT_PITCH_BEND:
            return "MIDI_EVENT_PITCH_BEND";
        case MIDI_EVENT_SYSEX:
            return "MIDI_EVENT_SYSEX";
        case MIDI_EVENT_SYSEX_SPLIT:
            return "MIDI_EVENT_SYSEX_SPLIT";
        case MIDI_EVENT_META:
            return "MIDI_EVENT_META";

        default:
            return "(unknown)";
    }
}

void PrintTrack(midi_track_t *track)
{
    midi_event_t *event;
    unsigned int i;

    for (i=0; i<track->num_events; ++i)
    {
        event = &track->events[i];

        if (event->delta_time > 0)
        {
            printf(english_language ?
                    "Delay: %i ticks\n" :
                    "Задержка: %i тик(ов)\n",
                    event->delta_time);
        }

        printf(english_language ?
               "Event type: %s (%i)\n" :
               "Тип события: %s (%i)\n",
               MIDI_EventTypeToString(event->event_type),
               event->event_type);

        switch(event->event_type)
        {
            case MIDI_EVENT_NOTE_OFF:
            case MIDI_EVENT_NOTE_ON:
            case MIDI_EVENT_AFTERTOUCH:
            case MIDI_EVENT_CONTROLLER:
            case MIDI_EVENT_PROGRAM_CHANGE:
            case MIDI_EVENT_CHAN_AFTERTOUCH:
            case MIDI_EVENT_PITCH_BEND:
                printf(english_language ?
                       "\tChannel: %i\n" :
                       "\tКанал: %i\n",
                       event->data.channel.channel);
                printf(english_language ?
                       "\tParameter 1: %i\n" :
                       "\tПараметр 1: %i\n",
                       event->data.channel.param1);
                printf(english_language ?
                        "\tParameter 2: %i\n" :
                        "\tПараметр 2: %i\n",
                        event->data.channel.param2);
                break;

            case MIDI_EVENT_SYSEX:
            case MIDI_EVENT_SYSEX_SPLIT:
                printf(english_language ?
                        "\tLength: %i\n" :
                        "\tДлина: %i\n",
                        event->data.sysex.length);
                break;

            case MIDI_EVENT_META:
                printf(english_language ?
                        "\tMeta type: %i\n" :
                        "\tМета-тип: %i\n",
                        event->data.meta.type);
                printf(english_language ?
                       "\tLength: %i\n" :
                       "\tДлина: %i\n",
                       event->data.meta.length);
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    midi_file_t *file;
    unsigned int i;

    if (argc < 2)
    {
        printf(english_language ?
               "Usage: %s <filename>\n" :
               "Используется: %s <filename>\n",
               argv[0]);
        exit(1);
    }

    file = MIDI_LoadFile(argv[1]);

    if (file == NULL)
    {
        printf(english_language ?
                "Failed to open %s\n" :
                "Ошибка открытия %s\n", 
                argv[1]);
        exit(1);
    }

    for (i=0; i<file->num_tracks; ++i)
    {
        printf(english_language ?
               "\n== Track %i ==\n\n" :
               "\n== Дорожка %i ==\n\n", i);

        PrintTrack(&file->tracks[i]);
    }

    return 0;
}

#endif

