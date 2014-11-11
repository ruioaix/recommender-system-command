/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdio.h>

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

/**
 * add by rui. the mt&mti data is generated with following args.
 * 1. unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
 * 2. init_by_array(init, length);
 * I don't need to init anymore before using get_i32_MTPR/get_i31_MTPR/... because 
 * of setting the two data static.
 * if you use the two data following, you will get same seriels random number.
 * if you need different random number, just set different init[N] and use 
 * init_by_array() to genrate different mt[N]. 
 */
//static unsigned long mt[N]; // the array for the state vector  
//static int mti=N+1; // mti==N+1 means mt[N] is not initialized 
static int mti=624;
static unsigned long mt[N]={
	2147483648ul, 1827812183ul, 1371430253ul, 3559376401ul, 4152304030ul, 3484546413ul, 
	904688886ul, 2640105624ul, 3033298696ul, 3386595201ul, 369478181ul, 2269089429ul, 
	618892536ul, 1404079710ul, 2891471128ul, 2871169702ul, 1020385029ul, 3311824836ul, 
	923090660ul, 1921041923ul, 1544466474ul, 685797265ul, 2465597578ul, 2346973074ul, 
	630894856ul, 665182002ul, 769175936ul, 3017123521ul, 3508859727ul, 1803296144ul, 
	2059566241ul, 4053644259ul, 3993140148ul, 741891261ul, 54278865ul, 20043431ul, 
	2445648452ul, 1044381999ul, 1773042707ul, 2768959228ul, 1818875112ul, 2035786986ul, 
	3458871676ul, 4190322778ul, 1552412821ul, 3743407409ul, 921020090ul, 3028219647ul, 
	1811282874ul, 458436201ul, 2261805142ul, 3540504881ul, 55523394ul, 3406035835ul, 
	3643554219ul, 160478601ul, 3131812209ul, 1750246847ul, 310524801ul, 538838761ul, 
	4097216171ul, 1424467795ul, 1736964303ul, 3924877158ul, 4050066274ul, 1284818847ul, 
	997608279ul, 1743645731ul, 3861638089ul, 1979190831ul, 331968179ul, 67771207ul, 
	937697508ul, 3340056593ul, 752755706ul, 1581536688ul, 2292813118ul, 822270257ul, 
	3518209714ul, 3540339067ul, 1356302093ul, 564364160ul, 3439306709ul, 4156257291ul, 
	2902329296ul, 2676847202ul, 199377087ul, 2700614214ul, 4256495779ul, 1042379751ul, 
	1902843850ul, 541663347ul, 1105719791ul, 2055488122ul, 1772382387ul, 1590461887ul, 
	1096731286ul, 4039442187ul, 1175370089ul, 1659062698ul, 53134572ul, 2821322629ul, 
	3865246561ul, 1928684207ul, 2238726924ul, 3291877865ul, 2486983120ul, 2591476417ul, 
	2633030347ul, 4263927136ul, 3370274455ul, 3018969306ul, 3342536519ul, 1048809092ul, 
	3887576647ul, 1798999130ul, 1882276042ul, 2314104910ul, 3303817434ul, 2251178951ul, 
	1354109516ul, 2944211425ul, 542518603ul, 3545156308ul, 292048272ul, 1667455762ul, 
	1824881537ul, 2291719689ul, 2601744913ul, 1475382386ul, 2151236187ul, 3551355316ul, 
	207179492ul, 4025262836ul, 4201541508ul, 4185568575ul, 1371095840ul, 2533214650ul, 
	750598898ul, 3336127010ul, 1112746635ul, 2168833184ul, 1866663264ul, 1008882524ul, 
	705705152ul, 434584641ul, 3720585473ul, 1359539617ul, 1186019465ul, 2997504282ul, 
	3279906530ul, 4174236237ul, 1111392757ul, 1874728325ul, 3994133610ul, 855552173ul, 
	1490712571ul, 770322226ul, 3509743848ul, 3097776221ul, 1669989606ul, 2981948555ul, 
	3420675648ul, 763522569ul, 1854726402ul, 1446282084ul, 4210797998ul, 3497860296ul, 
	1500567652ul, 4170419592ul, 375681337ul, 1685467682ul, 3252917671ul, 204735328ul, 
	725724149ul, 86204629ul, 3407749819ul, 1497038849ul, 1240663743ul, 1497563373ul, 
	330463645ul, 678281313ul, 555587055ul, 3420977290ul, 3864978040ul, 2598258963ul, 
	88433849ul, 706471606ul, 2204253466ul, 1207243547ul, 479433004ul, 2525229949ul, 
	3011716495ul, 3622461821ul, 2931310287ul, 994916916ul, 3209595751ul, 3887340275ul, 
	1441571561ul, 2496624432ul, 3592967859ul, 2170622192ul, 3964102663ul, 1750936864ul, 
	4286251765ul, 2410873793ul, 4056435468ul, 3778832465ul, 3568403537ul, 3195560039ul, 
	2447945345ul, 2289664522ul, 3125246309ul, 3254932065ul, 3412281324ul, 1070323682ul, 
	1454196965ul, 1242606593ul, 2025763526ul, 2480282382ul, 1785523412ul, 1678120983ul, 
	3466302448ul, 913688339ul, 2335682621ul, 2870884017ul, 3943203012ul, 3376940353ul, 
	821708821ul, 5329814ul, 2050965739ul, 4012960695ul, 4026282590ul, 848005093ul, 
	2090115245ul, 1732838459ul, 1701934203ul, 2618985574ul, 2392637351ul, 3610308264ul, 
	1162509741ul, 3772926898ul, 4026399774ul, 2801305136ul, 2182020938ul, 2540720293ul, 
	4177612013ul, 3758227323ul, 2141839466ul, 3457019126ul, 690868322ul, 598123635ul, 
	4220136616ul, 2063722767ul, 534543515ul, 3995640444ul, 3697772838ul, 605496025ul, 
	194542699ul, 4283492525ul, 3414550867ul, 2944993929ul, 3138370086ul, 2273260021ul, 
	1368435547ul, 328762456ul, 2810028600ul, 3571321502ul, 1782649452ul, 4008389438ul, 
	3374096016ul, 2070297302ul, 3548106024ul, 406229528ul, 420474145ul, 1345435476ul, 
	1454548380ul, 2158312864ul, 1379619651ul, 1353546123ul, 3811653577ul, 882683258ul, 
	2752377205ul, 3614226931ul, 1500298293ul, 2610950419ul, 1253921430ul, 2107280447ul, 
	2276719522ul, 1296486334ul, 3787958895ul, 3076802280ul, 1113584194ul, 1511963190ul, 
	4162049235ul, 3993023547ul, 3616161164ul, 1305747926ul, 3972166487ul, 1967370814ul, 
	920085398ul, 3066264196ul, 3270634177ul, 116308049ul, 3500602755ul, 2095466291ul, 
	374997835ul, 4013819058ul, 294797265ul, 2531304952ul, 1915167960ul, 667114423ul, 
	33740368ul, 2659595205ul, 1992038463ul, 1788663246ul, 1032288339ul, 2725176344ul, 
	965149701ul, 2340916620ul, 2880875099ul, 90563045ul, 550093497ul, 2205859613ul, 
	4113574329ul, 2611773242ul, 6224964ul, 2216769577ul, 2724275975ul, 1825208654ul, 
	1095989352ul, 1353654145ul, 4130469231ul, 2872700633ul, 3968090132ul, 3983530071ul, 
	3891778552ul, 253511543ul, 3451123488ul, 1858275584ul, 2348219340ul, 4164791526ul, 
	1546492002ul, 2318605776ul, 1965910834ul, 4016688232ul, 1661025458ul, 2551621891ul, 
	2559017942ul, 2999442539ul, 2626132036ul, 3855956683ul, 889284004ul, 4228792998ul, 
	2508936804ul, 2464411834ul, 3126525029ul, 1845662917ul, 1099875911ul, 230942152ul, 
	1316977638ul, 4287590380ul, 3960960001ul, 2035696777ul, 2084606597ul, 4185117397ul, 
	272555796ul, 1438814277ul, 3792058704ul, 2373818687ul, 328461098ul, 1933025545ul, 
	478993165ul, 2186693443ul, 2504363491ul, 2844647904ul, 4178236473ul, 174820621ul, 
	3894751753ul, 3928736686ul, 691843130ul, 3709350628ul, 2757608178ul, 1391782179ul, 
	1043240547ul, 2220523142ul, 2016183159ul, 2906513762ul, 3465827756ul, 3911890461ul, 
	2425950392ul, 948237040ul, 3362307558ul, 826526964ul, 1022776727ul, 1602110624ul, 
	495555482ul, 3374275041ul, 1251359676ul, 1255187341ul, 2187964004ul, 2260892791ul, 
	2998084135ul, 1359605011ul, 1974780261ul, 150018524ul, 729982456ul, 538716187ul, 
	2451949310ul, 1404163987ul, 323166556ul, 3575751708ul, 912151627ul, 1062492739ul, 
	136196234ul, 1525463811ul, 3934317698ul, 4059639798ul, 1714397141ul, 3799270074ul, 
	3933323871ul, 1950899325ul, 2202877835ul, 512028145ul, 121304123ul, 1747320652ul, 
	4025527049ul, 3203512622ul, 370863179ul, 1383153679ul, 338573713ul, 843668046ul, 
	3786076594ul, 3761568863ul, 1943691400ul, 4239756727ul, 2866888284ul, 927687523ul, 
	2835168303ul, 3639022486ul, 4177667412ul, 2454774443ul, 3330293413ul, 1073705099ul, 
	489962519ul, 2122918676ul, 3091912613ul, 2087072392ul, 2474785306ul, 4196422017ul, 
	1950304138ul, 1417647053ul, 3170449575ul, 2871308877ul, 2834888397ul, 2515213887ul, 
	706488188ul, 2765291661ul, 3342253292ul, 946613144ul, 2657539519ul, 1870033541ul, 
	2568978334ul, 2381242604ul, 2390291123ul, 1426830313ul, 1033183417ul, 1865206616ul, 
	3909956281ul, 3229647106ul, 567876042ul, 2632601739ul, 1787091399ul, 1136358267ul, 
	1707922556ul, 3600383805ul, 83493536ul, 3720481151ul, 1860566046ul, 1457764290ul, 
	1087693608ul, 2574737982ul, 3472233887ul, 1946429651ul, 657205714ul, 1561347349ul, 
	3458704973ul, 658970395ul, 2666292445ul, 4010435611ul, 2923502084ul, 2919668932ul, 
	1447403386ul, 272213229ul, 2379902797ul, 856095986ul, 390957008ul, 316624381ul, 
	37938916ul, 1312588871ul, 3443811924ul, 3563720170ul, 681138701ul, 3713652810ul, 
	1229426900ul, 45899309ul, 3319920329ul, 3184643346ul, 1733808374ul, 3218014532ul, 
	4131837143ul, 2708745467ul, 1925606777ul, 1070300412ul, 1857066639ul, 231709688ul, 
	89357594ul, 2807557743ul, 4246955725ul, 2648639203ul, 4014481964ul, 3609309784ul, 
	3413370774ul, 3362710738ul, 4117303240ul, 2975382313ul, 4244728709ul, 676814731ul, 
	1401643182ul, 4193290992ul, 1880982784ul, 3412352641ul, 432923266ul, 3204217730ul, 
	3640767186ul, 2748687591ul, 3238328348ul, 1816348975ul, 3454683239ul, 1788280760ul, 
	2390076397ul, 583783115ul, 749971462ul, 2070386333ul, 3684914363ul, 2505473379ul, 
	3781484919ul, 3251603932ul, 1061410633ul, 2657465574ul, 1047386942ul, 598203491ul, 
	3802852895ul, 3892183081ul, 650367471ul, 1888164348ul, 3790664944ul, 3285152048ul, 
	1772040010ul, 2779620173ul, 262034140ul, 4011996451ul, 172805695ul, 3112617504ul, 
	3698026131ul, 2749738982ul, 1338645063ul, 2883589338ul, 1171912066ul, 1463032348ul, 
	2303251030ul, 1690714656ul, 87045526ul, 3931593098ul, 1973669665ul, 2196833368ul, 
	3131780013ul, 2099423072ul, 3143196216ul, 2732157577ul, 3060002362ul, 3279855788ul, 
	2424755944ul, 3186596011ul, 892164315ul, 1948474334ul, 1752877034ul, 3954052922ul, 
	2895910728ul, 2605843797ul, 41504255ul, 2282306723ul, 2818684947ul, 2301112141ul, 
	125160123ul, 16831279ul, 2001639563ul, 2988100720ul, 2804013521ul, 1917204704ul, 
	2217119131ul, 747297802ul, 57526359ul, 1150692435ul, 1127421285ul, 1638007393ul, 
	2357393576ul, 1363062508ul, 112227006ul, 1305326547ul, 2699963315ul, 3027707384ul, 
	2567899981ul, 539524824ul, 1322391149ul, 4115045373ul, 2610809222ul, 1511354791ul, 
	1747171732ul, 2430429081ul, 2427698197ul, 668479568ul, 1616806017ul, 77518867ul 
};

/* initializes mt[N] with a seed */
static void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
static void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k>0; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k>0; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

#include <time.h>
void set_timeseed_MTPR(void) {
	unsigned long t=(unsigned long)time(NULL);
	unsigned long init[4]={t, 0x234, 0x345, 0x456};
	int length=4;
	init_by_array(init, length);
}

void set_seed_MTPR(unsigned long seed) {
	unsigned long init[4]={seed, 0x234, 0x345, 0x456};
	int length=4;
	init_by_array(init, length);
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long get_i32_MTPR(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long get_i31_MTPR(void)
{
    return (long)(get_i32_MTPR()>>1);
}

/* generates a random number on [0,1]-real-interval */
double get_d01_MTPR(void)
{
    return (double)get_i32_MTPR()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double get_d0_MTPR(void)
{
    return (double)get_i32_MTPR()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double get_d_MTPR(void)
{
    return (double)(((double)get_i32_MTPR()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double get_d530_MTPR(void) 
{ 
    unsigned long a=get_i32_MTPR()>>5, b=get_i32_MTPR()>>6; 
    return (double)(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

