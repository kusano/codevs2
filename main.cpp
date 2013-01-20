#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <process.h>
#include "field.h"

using namespace std;


//  パラメタ

//  スレッド数
#define     THREAD_NUM      4

//  スレッドあたりの探索の実行回数
#define     TRIAL_NUM_S     2
#define     TRIAL_NUM_M     4
#define     TRIAL_NUM_L     2

//  探索深さ
#define     DEPTH_S         2
#define     DEPTH_M         1
#define     DEPTH_L         1

//  評価関数
#define     STRING1(s)      STRING2(s)
#define     STRING2(s)      #s
#define     EVALUATE_S      \
    long long value = chain*256 + log2(score);                  \
    value -= max(F.getmaxheight()-F.getminheight()-3,0)*4096;   \
    value -= max(F.getblocknum()-chain*250/100,0)*64;           \
    return value;
#define     SEVALUATE_S     STRING1(EVALUATE_S)

#define     EVALUATE_M      \
    long long value = chain*256 + log2(score);                  \
    value -= max(F.getmaxheight()-F.getminheight()-3,0)*2048;   \
    value -= max(F.getblocknum()-chain*300/100,0)*32;           \
    return value;
#define     SEVALUATE_M     STRING1(EVALUATE_M)

#define     EVALUATE_L      \
    long long value = chain*256 + log2(score);                  \
    value -= max(F.getmaxheight()-F.getminheight()-4,0)*2048;   \
    value -= max(F.getblocknum()-chain*500/100,0)*32;           \
    return value;
#define     SEVALUATE_L     STRING1(EVALUATE_L)



static int                          W, H, T, S, N;
static vector<vector<vector<int>>>  pack;
static vector<vector<MOVE>>         move;
static vector<long long>            score;
static vector<bool>                 finish;



static void __cdecl thread( void *arglist );
template<int W, int H, int T, int S, int N, int P>
static int solve( int id, vector<vector<vector<int>>> pack, vector<MOVE> *rmove, long long *rscore );
template<int W, int H, int T, int S, int N, int P>
static long long search( Field<W,H,T,S,N,P> &F, int d );
static int log2( long long n ) {int i=0;while(n<1LL<<i)i++;return i;}



//  main
int main()
{
    //  読み込み
    cin>>W>>H>>T>>S>>N;
    pack = vector<vector<vector<int>>>( N, vector<vector<int>>(T,vector<int>(T)) );
    for ( int n=0; n<N; n++ )
    {
        for ( int y=0; y<T; y++ )
        for ( int x=0; x<T; x++ )
            cin>>pack[n][T-y-1][x];
        string end;
        cin>>end;
    }

    //  スレッド起動
    int tnum = THREAD_NUM*(W==10?TRIAL_NUM_S:W==15?TRIAL_NUM_M:TRIAL_NUM_L);
    ::move = vector<vector<MOVE>>( tnum, vector<MOVE>(N) );
    score = vector<long long>( tnum );
    finish = vector<bool>( THREAD_NUM );

    for ( int i=0; i<THREAD_NUM; i++ )
        _beginthread( thread, 0, (void *)i );

    //  スレッドの終了を待機
    while ( true )
    {
        bool f = true;
        for ( int i=0; i<THREAD_NUM; i++ )
            if ( !finish[i] )
                f = false;
        if ( f )
            break;

        #pragma warning(disable:4996)
        _sleep( 1000 );
    }

    //  最高点の試行を解として出力
    int best = 0;
    for ( int i=1; i<tnum; i++ )
        if ( score[i] > score[best] )
            best = i;

    for ( int n=0; n<N; n++ )
        cout << ::move[best][n].X << " " << ::move[best][n].R << endl;

    return 0;
}



//  スレッドのエントリーポイント
void __cdecl thread( void *arglist )
{
    int tid = (int)arglist;

    int tn = W==10 ? TRIAL_NUM_S : W==15 ? TRIAL_NUM_M : TRIAL_NUM_L;

    for ( int i=0; i<tn; i++ )
    {
        int id = i*THREAD_NUM + tid;
        vector<MOVE> m;
        long long s;

        switch ( W )
        {
        case 10:  solve< 10, 16+4, 4, 10, 1000, 25>( id, pack, &m, &s );  break;
        case 15:  solve< 15, 23+4, 4, 20, 1000, 30>( id, pack, &m, &s );  break;
        case 20:  solve< 20, 36+5, 5, 30, 1000, 35>( id, pack, &m, &s );  break;
        }

        for ( int n=0; n<N; n++ )
            ::move[id][n] = m[n];
        score[id] = s;
    }

    finish[tid] = true;

    _endthread();
}



//  計算
template<int W, int H, int T, int S, int N, int P>
int solve( int id, vector<vector<vector<int>>> pack, vector<MOVE> *rmove, long long *rscore )
{
    //  ログファイル
    string fname = "log\\log_";

    if ( W==10 ) fname += "s_";
    if ( W==15 ) fname += "m_";
    if ( W==20 ) fname += "l_";

    time_t ctm = time(NULL);
    tm ltm;
    localtime_s( &ltm, &ctm );
    char tmp[32];
    strftime( tmp, sizeof tmp, "%Y%m%d_%H%M%S_", &ltm );
    fname += tmp;

    sprintf( tmp, "%02d.txt", id );
    fname += tmp;

    ofstream log(fname);

    //  乱数初期化（たぶんVC++ならスレッドセーフ）
    srand(id);

    //  パラメタを保存
    if ( W==10 )
    {
        log << "Depth: " << DEPTH_S << endl;
        log << "Evaluate: " << endl;
        log << SEVALUATE_S << endl;
    }
    if ( W==15 )
    {
        log << "Depth: " << DEPTH_M << endl;
        log << "Evaluate: " << endl;
        log << SEVALUATE_M << endl;
    }
    if ( W==20 )
    {
        log << "Depth: " << DEPTH_L << endl;
        log << "Evaluate: " << endl;
        log << SEVALUATE_L << endl;
    }

    //  パックを保存
    for ( int n=0; n<N; n++ )
        for ( int y=0; y<T; y++ )
        {
            for ( int x=0; x<T; x++ )
                log << " " << pack[n][T-y-1][x];
            log << endl;
        }

    Field<W,H,T,S,N,P> F(pack);

    MOVE move[N];
    long long maxscore[N];      //  各ステップで得られる最大スコア
    int prewind = 0;            //  前回巻き戻したステップ　ここより前には巻き戻さない

    for ( int n=0; n<N; n++ )
    {
        vector<pair<long long,MOVE>> m;

        long long mscore = -99999999;
        int mchain = -1;
        
        for ( int x=-T; x<W; x++ )
        for ( int r=0; r<4; r++ )
        if ( F.check(MOVE(x,r)) )
        {
            long long s0 = F.getscore();
            int c;

            F.move(MOVE(x,r),&c);

            long long s1 = F.getscore();
            long long s2 = -99999999LL;
            if ( s1>=0 )
                s2 = search( F, W==10 ? DEPTH_S : W==15 ? DEPTH_M : DEPTH_L );
                
            mscore = max( mscore, s1 );
            mchain = max( mchain, c );

            m.push_back(make_pair(s2,MOVE(x,r)));
            
            F.undo();
        }

        random_shuffle(m.begin(),m.end());
        sort(m.begin(),m.end());

        move[n] = m.back().second;

        F.move(m.back().second);

        maxscore[n] = mscore;

        log << n << " " << (double)mscore << " " << mchain << " " << clock() << endl;
        log << F.tostring(true) << endl;

        //  最終ステップなら点数が最大になるところまで巻き戻す
        if ( n==N-1 )
        {
            int rw;
            long long m = 0;
            for ( int i=0; i<=n; i++ )
                if ( maxscore[i] > m )
                    m = maxscore[i],
                    rw = i;

            if ( rw<n && prewind<rw )
            {
                F.undo();
                while ( n>rw )
                    F.undo(),
                    n--;

                //  今度は点数が最大になるものを選択
                vector<pair<long long,MOVE>> m;

                for ( int x=-T; x<W; x++ )
                for ( int r=0; r<4; r++ )
                if ( F.check(MOVE(x,r)) )
                {
                    F.move(MOVE(x,r));
                    m.push_back(make_pair(F.getscore(),MOVE(x,r)));
                    F.undo();
                }

                sort(m.begin(),m.end());

                move[n] = m.back().second;
                F.move(m.back().second);
                maxscore[n] = m.back().first;

                prewind = n;

                log << n << " " << (double)m.back().first << " " << clock() <<  endl;
            }

            //  大連鎖以降の連鎖はスコアにほとんど影響が無いので、終了で良いのでは
            break;
        }
    }

    //  出力
    for ( int n=0; n<N; n++ )
        log << move[n].X << " " << move[n].R << endl;
    log << id << endl;
    log << F.tostring() << endl;

    *rmove = vector<MOVE>(move,move+N);
    *rscore = F.getscore();

    return 0;
}



//  探索
template<int W, int H, int T, int S, int N, int P>
long long search( Field<W,H,T,S,N,P> &F, int d )
{
    if ( d==0 )
    {
        long long current = F.getscore();
        long long score;
        int chain;
        F.getideal( &score, &chain );

        if ( W==10 )
        {
            EVALUATE_S
        }
        if ( W==15 )
        {
            EVALUATE_M
        }
        if ( W==20 )
        {
            EVALUATE_L
        }
        
        return -1;
    }

    long long s = -99999999LL;

    for ( int x=-T; x<W; x++ )
    for ( int r=0; r<4; r++ )
    if ( F.check(MOVE(x,r)) )
    {
        F.move(MOVE(x,r));
        s = max( s, search(F,d-1) );
        F.undo();
    }
    return s;
}
