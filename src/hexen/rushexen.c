//
// Copyright(C) 2020 Dasperal
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



#include "rushexen.h"

#define US "YTJ;BLFYYFZ CNHJRF! 'NJ <FU!" // ����������� ������! ��� ���!

const char* Hexen_Map_01_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    "LDTHM PF,KJRBHJDFYF", // ����� �������������
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_02_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "GHBDTNCNDE.< CVTHNYSQ", // ������������ ��������
    US,
    "UJNJD KB NS EVTHTNM?", // ����� �� �� �������,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "LDTHM JNRHSNF E CNHF;F KMLF", // ����� ������� � ������ ����
    US,
    US,
    US
};

const char* Hexen_Map_03_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "CTQXFC 'NJN GENM PFRHSN", // ������ ���� ���� ������
    US,
    US
};

const char* Hexen_Map_04_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "GJKJDBYF UJKJDJKJVRB HFPUFLFYF>>>", // �������� ����������� ������������
    ">>>YF CTVB GJHNFKF[", // ����� ���� ��������
    "NHTNM UJKJDJKJVRB HFPUFLFYF>>>", // ����� ����������� ������������
    "KTCNYBWF DJPLDBUYTNCZ YF CTVB GJHNFKF[", // �������� ������������ �� ���� ��������
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_05_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "NHTNM UJKJDJKJVRB HFPUFLFYF>>>", // ����� ����������� ������������
    ">>>YF CTVB GJHNFKF[", // ����� ���� ��������
    "KTCNYBWF DJPLDBUYTNCZ YF CTVB GJHNFKF[", // �������� ������������ �� ���� ��������
    "JCNFKCZ TOT JLBY GTHTRK.XFNTKM>>>", // ������� ��� ���� ����������������
    "RFVTYYFZ GHTUHFLF JNCNEGBN>>>", // �������� �������� �����������
    US,
    US
};

const char* Hexen_Map_08_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "JLYF ITCNFZ UJKJDJKJVRB HFPUFLFYF>>>", // ���� ������ ����������� ������������
    ">>>D KTCE NTYTQ", // ���� ���� �����
    US,
    US,
    "LDTHM PF,KJRBHJDFYF BPYENHB", // ����� ������������� �������
    "CKSITY PDER JNRHSDF.OTQCZ LDTHB", // ������ ���� ������������� �����
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_09_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "JLYF ITCNFZ UJKJDJKJVRB HFPUFLFYF>>>", // ���� ������ ����������� ������������
    ">>>D KTCE NTYTQ", // ���� ���� �����
    US,
    US,
    US
};

const char* Hexen_Map_10_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "JLYF ITCNFZ UJKJDJKJVRB HFPUFLFYF>>>", // ���� ������ ����������� ������������
    ">>>D KTCE NTYTQ", // ���� ���� �����
    US,
    US,
    US,
    US
};

const char* Hexen_Map_11_StringTable[] = {
    "JCNFKJCM 'NNBYJD: ", // �������� �������� 
    "CKBIRJV LJKUJ< UJNJDMCZ R CVTHNB!", // ������� ������ �������� � ������!
    US,
    US,
    US,
    US,
    US,
    "LDTHM JNRHJTNCZ YF GJRBYENJQ PFCNFDT", // ����� ��������� �� ��������� �������
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_12_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "CTQXFC 'NF LDTHM PFRHSNF", // ������ ��� ����� �������
    US,
    US,
    US
};

const char* Hexen_Map_13_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "VJB GHBCKE;YBRB XE.N NDJ. RHJDM< XTKJDTR", // ��� ����������� ���� ���� ������ �������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_21_StringTable[] = {
    "GHJ[JL ,ELTN JNRHSN DJPKT DBCTKBWS", // ������ ����� ������ ����� ��������
    US,
    "LDTHM PF,KJRBHJDFYF BPYENHB", // ����� ������������� �������
    US,
    US,
    US,
    US
};

const char* Hexen_Map_22_StringTable[] = {
    US,
    US,
    US,
    "GKJOFLRF JGECNBKFCM D WTYNHFKMYJQ ,FIYT", // �������� ���������� � ����������� �����
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "NS CKBIRJV PFBUHFKCZ< CVTHNYSQ>>>", // �� ������� ���������� �����������
    US,
    "GHBIKJ DHTVZ PFDTHIBNM NDJ. GFHNB.", // ������ ����� ��������� ���� ������
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_23_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "BP CTDTHYJUJ PFKF LJYJCBNCZ PDER>>>", // �� ��������� ���� ��������� �������
    US,
    US,
    US
    // TODO
};

const char* Hexen_Map_27_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "GHTRKJYBCM GHTLJ VYJQ<", // ���������� ����� �����
    US,
    "B< VJ;TN ,SNM< Z ,ELE VBKJCTHLTY", // �� ����� ����� � ���� ����������
    US,
    US,
    US,
    US,
    US,
    US,
    US
    // TODO
};

const char* Hexen_Map_28_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "JLYF LTDZNFZ UJKJDJKJVRB HFPUFLFYF>>>", // ���� ������� ����������� ������������
    ">>>D CTVBYFHBB THTCBFH[F", // ���� ��������� ���������
    US,
    US,
};

const char* Hexen_Map_30_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "JLYF LTDZNFZ UJKJDJKJVRB HFPUFLFYF>>>", // ���� ������� ����������� ������������
    ">>>D CTVBYFHBB THTCBFH[F", // ���� ��������� ���������
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_34_StringTable[] = {
    US,
    "JLYF LTDZNFZ UJKJDJKJVRB HFPUFLFYF>>>", // ���� ������� ����������� ������������
    ">>>D CTVBYFHBB THTCBFH[F", // ���� ��������� ���������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_Map_35_StringTable[] = {
    "DHFNF PFRHSKBCM>>>", // ����� ������������
    "CLTKFQ CDJQ DS,JH", // ������ ���� �����
    US,
    "LDTHM PFGTHNF BPYENHB", // ����� ������� �������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "LJCNFNJXYJ KB NS CBKTY<", // ���������� �� �� ������
    US,
    "XNJ,S CHFPBNMCZ CJ CDJBVB YFCNFDYBRFVB?", // ����� ��������� �� ������ ������������,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_33_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "NS GJCVTK CHF;FNMCZ D RJVYFNT J;BLFYBZ?", // �� ������ ��������� � ������� ��������,
    "NFR EVHB ;T!" // ��� ���� ��!
};

const char* Hexen_DK_Map_41_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    "GJNJR DJLS JCNFYJDKTY", // ����� ���� ����������
    "GJNJR DJLS GHTUHF;LFTN GENM", // ����� ���� ����������� ����
    "LDTHM JNRHSKFCM D XFCJDYT", // ����� ��������� � �������
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_42_StringTable[] = {
    US,
    US,
    US,
    US,
    "RFR CNHFYYJ>>>", // ��� ����������
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_44_StringTable[] = {
    US,
    "JCNFKJCM NHB XFCNB UJKJDJKJVRB", // �������� ��� ����� �����������
    "JCNFKJCM LDT XFCNB UJKJDJKJVRB", // �������� ��� ����� �����������
    "JCNFKFCM JLYF XFCNM UJKJDJKJVRB", // �������� ���� ����� �����������
    "UJKJDJKJVRF HFPUFLFYF", // ����������� ���������
    US,
    "UJKJDJKJVRF YT HFPUFLFYF", // ����������� �� ���������
    US,
    "GJK CJDCTV GHJUYBK!", // ��� ������ �������!
    US,
    "JLYF NHTNM UJKJDJKJVRB HFPUFLFYF", // ���� ����� ����������� ���������
    "LDT NHTNB UJKJDJKJVRB HFPUFLFYS", // ��� ����� ����������� ���������
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_45_StringTable[] = {
    US,
    "YTGJLFKTRE HFPLFTNCZ PDER>>>", // ���������� ��������� �������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
    // TODO
};

const char* Hexen_DK_Map_46_StringTable[] = {
    "UJNJDJ>>>", // ���������
    "UJKJDJKJVRF YT HFPUFLFYF", // ����������� �� ���������
    "Z NT,Z GHTLEGHT;LF.>>>", // � ���� ���������������
    "YT CKBIRJV KB NS EGHZVSQ?", // �� ������� �� �� �������,
    "B YT CKBIRJV-NJ HFPEVYSQ!", // � �� �������-�� ��������!
    US,
    US,
    US,
    "JLYF XTNDTHNFZ 'NJQ UJKJDJKJVRB HFPUFLFYF", // ���� ��������� ���� ����������� ���������
    "GKJ[JQ DS,JH>>>", // ������ ��������
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_47_StringTable[] = {
    US,
    US,
    "CBVDJKS YT CJDGFLF.N", // ������� �� ���������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_48_StringTable[] = {
    US,
    US,
    "C 'NJQ CNJHJYS LDTHM YT JNRHSNM", // � ���� ������� ����� �� �������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_50_StringTable[] = {
    US,
    "LDTHM PF,KJRBHJDFYF CYFHE;B", // ����� ������������� �������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_51_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    "CDZNJNFNCNDJ!", // ������������!
    "NS JCRDTHYBK VJUBKE 'HBRF!!", // �� ��������� ������ �����!!
    "B EVHTIM PF 'NJ CNHFIYJQ CVTHNM.!!!", // � ������ �� ��� �������� �������!!!
    "JLYF NHTNM UJKJDJKJVRB HFPUFLFYF", // ���� ����� ����������� ���������
    "LDT NHTNB UJKJDJKJVRB HFPUFLFYS", // ��� ����� ����������� ���������
    "CRKTG JNRHSN", // ����� ������
    "JGFCFQCZ UHJ,YBWS GFERF>>>", // �������� �������� ��������
    US,
    "CYFHE;B CKSITY PDER GJLYBVF.OTUJCZ RFVYZ", // ������� ������ ���� �������������� �����
    "&XEDCNDETIM KB NS CT,Z DTPEXBV?&", // ?���������� �� �� ���� �������,?
    "YTGHFDBKMYJT GHTLGJKJ;TYBT!", // ������������ �������������!
    "GHFDBKMYJT GHTLGJKJ;TYBT", // ���������� �������������
    "&VJ;TIM YFGBCFNM PF VTYZ DCT CRHBGNS?&", // ?������ �������� �� ���� ��� �������,?
    "&YT NHJUFQ VJ. DRECYZIRE&", // ?�� ������ ��� ���������?
    "&JCNHTYMRJ ?!?!?!&", // ?��������� ,!,!,!?
    "&GJLFQ-RF VYT CF[FH< LTNRF&", // ?�����-�� ��� ������ �����?
    "&FUF-F-F-F>>>&", // ?���-�-�-����?
    "&ABKMV YF XFC?&", // ?����� �� ���,?
    "&E VTYZ LF;T YTN CDJTUJ YFLUHJ,BZ (R>A>)&", // ?� ���� ���� ��� ������ ��������� (����)?
    "LF YT GHJKMTNCZ RHJDM>>>", // �� �� ��������� ��������
    "B LF YT GJLYBVTNCZ HERF DJ UYTDT>>>", // � �� �� ���������� ���� �� ��������
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_52_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "RNJ JCVTKBKCZ GJNHTDJ;BNM YFI GJRJQ?!", // ��� ��������� ����������� ��� �����,!
    "GENM JNRHSN", // ���� ������
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_53_StringTable[] = {
    US,
    US,
    "JCNFKJCM GTHTRK.XFNTKTQ: ", // �������� ���������������
    "",
    "JCNFKCZ DCTUJ JLBY GTHTRK.XFNTKM", // ������� ����� ���� �������������
    "", //TODO
    "DS[JL JNRHSN", // ����� ������
    US,
    US,
    US
};

const char* Hexen_DK_Map_54_StringTable[] = {
    US,
    "C 'NJQ CNJHJYS LDTHM YT JNRHSNM", // � ���� ������� ����� �� �������
    US,
    US,
    "GENM D WBNFLTKM JNRHSN>>>", // ���� � �������� ���������
    ">>>JCVTKBIMCZ KB NS DJQNB?", // ������������� �� �� �����,
    US,
    US,
    US,
    "LDTHM JNRHJTNCZ>>>", // ����� ������������
    ">>>D XFCJDYT", // ���� �������
    "VJCN DJPLDBUYTNCZ>>>", // ���� ���������������
    ">>>YF ,JQYT", // ����� �����
    "KTCNYBWF DJPLDBUYTNCZ>>>", // �������� ���������������
    ">>>E NTVYJUJ CNHF;F", // ���� ������� ������
    "ITCNTHYZ ECNFYJDKTYF", // �������� �����������
    "ITCNTHYTQ ECNFYJDKTYJ: ", // ��������� ������������ 
    "GHTUHFLF GJLYBVTNCZ>>>", // �������� �������������
    ">>>D RKJFRT", // ���� ������
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_55_StringTable[] = {
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    "LDTHM PF,KJRBHJDFYF BPYENHB", // ����� ������������� �������
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_56_StringTable[] = {
    "NS GJCVTK JUHF,BNM VJUBKE>>>", // �� ������ �������� ���������
    ">>>DTH[JDYJUJ GFKFXF?", // ������������� ������,
    "UJNJDMCZ R CVTHNB", // �������� � ������
    US,
    US,
    US,
    US,
    US
};

const char* Hexen_DK_Map_59_StringTable[] = {
    US,
    "JCNFKJCM YFQNB GTHTRK.XFNTKTQ: ", // �������� ����� ��������������� 
    "",
    "JCNFKCZ GJCKTLYBQ GTHTRK.XFNTKM", // ������� ��������� �������������
    "", //TODO
    "GENM R ,FIYT JNRHSN", // ���� � ����� ������
    US,
    US
};

const char* Hexen_DK_Map_60_StringTable[] = {
    US,
    US,
    US,
    "GJHNFK JNRHSN>>>", //������ ���������
    US,
    US,
    US,
    US,
    US,
    US,
    US
};

const char** stringTables[] = {
    NULL, //MAP00
    Hexen_Map_01_StringTable,
    Hexen_Map_02_StringTable,
    Hexen_Map_03_StringTable,
    Hexen_Map_04_StringTable,
    Hexen_Map_05_StringTable,
    NULL, //MAP06
    NULL, //MAP07
    Hexen_Map_08_StringTable,
    Hexen_Map_09_StringTable,
    Hexen_Map_10_StringTable,
    Hexen_Map_11_StringTable,
    Hexen_Map_12_StringTable,
    Hexen_Map_13_StringTable,
    NULL, //MAP14
    NULL, //MAP15
    NULL, //MAP16
    NULL, //MAP17
    NULL, //MAP18
    NULL, //MAP19
    NULL, //MAP20
    Hexen_Map_21_StringTable,
    Hexen_Map_22_StringTable,
    Hexen_Map_23_StringTable,
    NULL, //MAP24
    NULL, //MAP25
    NULL, //MAP26
    Hexen_Map_27_StringTable,
    Hexen_Map_28_StringTable,
    NULL, //MAP29
    Hexen_Map_30_StringTable,
    NULL, //MAP31
    NULL, //MAP32
    NULL, //MAP33
    Hexen_Map_34_StringTable,
    Hexen_Map_35_StringTable,
    NULL, //MAP36
    NULL, //MAP37
    NULL, //MAP38
    NULL, //MAP39
    NULL, //MAP40
    Hexen_DK_Map_41_StringTable,
    Hexen_DK_Map_42_StringTable,
    NULL, //MAP43
    Hexen_DK_Map_44_StringTable,
    Hexen_DK_Map_45_StringTable,
    Hexen_DK_Map_46_StringTable,
    Hexen_DK_Map_47_StringTable,
    Hexen_DK_Map_48_StringTable,
    NULL, //MAP49
    Hexen_DK_Map_50_StringTable,
    Hexen_DK_Map_51_StringTable,
    Hexen_DK_Map_52_StringTable,
    Hexen_DK_Map_53_StringTable,
    Hexen_DK_Map_54_StringTable,
    Hexen_DK_Map_55_StringTable,
    Hexen_DK_Map_56_StringTable,
    NULL, //MAP57
    NULL, //MAP58
    Hexen_DK_Map_59_StringTable,
    Hexen_DK_Map_60_StringTable
};

const char** GetRusStringTable(int map)
{
    if (isDK && map < 41) // DK DM maps
    {
        if (map == 33)
        {
            return Hexen_DK_Map_33_StringTable;
        }
        else
        {
            return NULL;
        }
    }
    return stringTables[map]; //Hexen original, DK story, Hexen Demo(String tables equal to Hexen original)
}
