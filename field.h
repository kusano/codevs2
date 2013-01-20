#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

//  ����
struct MOVE
{
    int         X, R;
                MOVE() {}
                MOVE( int x, int r ): X(x), R(r) {}
    
    bool operator<(const MOVE &m) const { return false; }
};

//  �Ֆ�
//  �R���p�C����Ɋe�p�����^���萔�ɂȂ����ق��������i��������Ȃ��j�̂ŁA
//  �N���X�e���v���[�g��p���Ď���
template<
    int         W,                  //  ��
    int         H,                  //  �����i���͂�H+T�j
    int         T,                  //  �p�b�N�̈�ӂ̒���
    int         S,                  //  ������l
    int         N,                  //  �p�b�N��
    int         P>                  //  �_���̃p�����^
class Field 
{
    const static long long INF = 1LL<<60;   //  ������
    const static int    pspin[4][T*T];      //  �p�b�N��]
    const static int    linenum = 3*(W+H)-2;//  �t�B�[���h���̒�����
    const static int    line[linenum][4];   //  �t�B�[���h���̒���
    const static int    pos2line[W*H][4];   //  �u���b�N���܂܂�钼��

    char        pack[N][T*T];       //  �p�b�N

    char        F[W*H];             //  �t�B�[���h
    int         step;               //  ���݂̃X�e�b�v��
    long long   score;              //  ���݂̃X�R�A
    int         maxchain;           //  �ő�A����
    int         blocknum;           //  ���݂̃u���b�N���i�����ז��j
    int         height[W];          //  �e��̍���
    
    int         pleft[N][4];        //  �ō��̃p�b�N�����ʒu
    int         pright[N][4];       //  �ŉE�̃p�b�N�����ʒu

    char        hist_F[N][W*H];     //  �t�B�[���h����
    long long   hist_score[N];      //  �X�R�A����
    int         hist_maxchain[N];   //  �ő�A��������
    int         hist_blocknum[N];   //  �u���b�N������
    int         hist_height[N][W];  //  ��������

    void        move( const char p[T*T], int xpos, int *chain );
public:
                Field( vector<vector<vector<int>>> p );
    bool        check( MOVE m ) const;
    void        move( MOVE m, int *chain=NULL );
    void        undo();
    long long   getscore() const { return score; }
    void        getideal( long long *score=NULL, int *chain=NULL,
                          int *bpos=NULL, int *bval=NULL );
    int         getblocknum() const { return blocknum; }
    int         getmaxheight() const { return *max_element(height,height+W); }
    int         getminheight() const { return *min_element(height,height+W); };
    string      tostring( bool detail=false ); //const;
};
