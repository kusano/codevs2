/*

��: W=5, H=8, T=2

���t�B�[���h

	 T�� 45 46 47 48 49
	  �� 40 41 42 43 44
	 H�� 35 36 37 38 39
		 30 31 32 33 34
		 25 26 27 28 29
		 20 21 22 23 24
		 15 16 17 18 19
		 10 11 12 13 14
		  5  6  7  8  9
	  ��  0  1  2  3  4
		�� W         ��
�@	 ���v���O��������H�͓��͂�H+T

���p�b�N
     T��  2  3
      ��  0  1
        �� T��

����]
	�p�b�NP����]R�����Ƃ��̈ʒup�ɂ���u���b�N��
	  P[pspin[R][p]]
	�œ�����B

	pspini[4][T*T] = {
	    { 0, 1, 2, 3 },
		{ 1, 3, 0, 2 },
		{ 3, 2, 1, 0 },
		{ 2, 0, 3, 1 }}

�����C��
�@  �����̍ۂ́A�c�E���E�΂߂̃��C���ɑ�����u���b�N��
    ��������ŕ\����̂ŁA�����E�����̎��̍��E�����ŊǗ�����B
    �����Ɏg���񂷂��߁A�ŏ���W�{�͏c�����B

    line[][4] = {
		{ 0,50, 5}, { 1,51, 5}, �c, { 4,54, 5},  //  W�{
		{ 0, 5, 1}, { 5,10, 1}, �c, {45,50, 1},  //  H�{
		{ 4,10, 6}, { 3,15, 6}, �c, {45,51, 6},  //  W+H-1�{
		{ 0, 4, 4}, { 1, 9, 4}, �c, {49,53, 4}}  //  W+H-1�{

�����C���t�ϊ�
    �e�u���b�N�������郉�C���B
	�ŏ��̗v�f�͏c�����B

	pos2line[][4] = {
	    { 0, 5,19,29}, {1,5,18,30}, �c };
*/

#include	"field.h"
#include	<cstdio>
#include	<iostream>

using namespace std;



//	������
//	p[n][y][x]: �p�b�N�A������0,0
template<int W, int H, int T, int S, int N, int P>
Field<W,H,T,S,N,P>::Field( vector<vector<vector<int>>> p )
{
	for ( int i=0; i<N; i++ )
		for ( int y=0; y<T; y++ )
		for ( int x=0; x<T; x++ )
			pack[i][y*T+x] = (char)p[i][y][x];

	memset( F, 0, sizeof F );
	step = 0;
	score = 0LL;
	maxchain = 0;
	blocknum = 0;
	for ( int i=0; i<W; i++ )
		height[i] = 0;

	for ( int i=0; i<N; i++ )
	for ( int R=0; R<4; R++ )
	{
		int l = T;
		int r = 0;
		for ( int p=0; p<T*T; p++ )
		if ( pack[i][pspin[R][p]]>0 )
			l = min( l, p%T ),
			r = max( r, p%T );
		pleft[i][R] = -l;
		pright[i][R] = W-r-1;
	}
}



//	m���͈͓����ǂ����`�F�b�N
template<int W, int H, int T, int S, int N, int P>
bool Field<W,H,T,S,N,P>::check( MOVE m ) const
{
	return pleft[step][m.R] <= m.X && m.X <= pright[step][m.R];
}



//	�w��ʒu�Ƀp�b�N�𗎂Ƃ�
template<int W, int H, int T, int S, int N, int P>
void Field<W,H,T,S,N,P>::move( MOVE m, int *chain )
{
	char p[T*T];
	for ( int i=0; i<T*T; i++ )
		p[i] = pack[step][pspin[m.R][i]];

	move( p, m.X, chain );
}



//	1��߂�
template<int W, int H, int T, int S, int N, int P>
void Field<W,H,T,S,N,P>::undo()
{
	step--;
	if ( step<N )
	{
		memcpy( F, hist_F[step], sizeof F );
		score = hist_score[step];
		maxchain = hist_maxchain[step];
		blocknum = hist_blocknum[step];
		memcpy( height, hist_height[step], sizeof height );
	}
}



//	�D���ȏꏊ�ɍD���ȃu���b�N��1���Ƃ����瓾����
//	�ő�X�R�A�ƍő�A�����A���̈ʒu�A�u���b�N�̐������擾
template<int W, int H, int T, int S, int N, int P>
void Field<W,H,T,S,N,P>::getideal( long long *score, int *chain, int *bpos, int *bval )
{
	if ( step>=N )
	{
		if ( score!=NULL ) *score = getscore();
		if ( chain!=NULL ) *chain = 0;
		if ( bpos!=NULL ) *bpos = 0;
		if ( bval!=NULL ) *bval = 0;
		return;
	}

	long long s = -INF;
	int c = -1;

	char p[T*T] = {0};

	for ( int x=0; x<W; x++ )
	for ( int v=1; v<S; v++ )
	{
		p[0] = v;
		int t;

		move( p, x, &t );

		s = max( s, getscore() );
		if ( t>c )
		{
			c = t;
			if ( bpos!=NULL )  *bpos = x;
			if ( bval!=NULL )  *bval = v;
		}

		undo();
	}

	if ( score!=NULL ) *score = s;
	if ( chain!=NULL ) *chain = c;

	return;
}



//	�Ֆʂ𕶎���ɕϊ�
//	detail���^�Ȃ�A�����Ȃǂ��o��
template<int W, int H, int T, int S, int N, int P>
string Field<W,H,T,S,N,P>::tostring( bool detail/*=false*/ )// const
{
	string R;
	char tmp[32];

	sprintf( tmp, "Turn: %d\n", step+1 );  R += tmp;
	sprintf( tmp, "Score: %g\n", (double)score );  R += tmp;
	sprintf( tmp, "MaxChain: %d\n", maxchain );  R += tmp;

	if ( detail )
	{
		int bpos, bval, chain;
		long long score;
		getideal( &score, &chain, &bpos, &bval );
		sprintf( tmp, "Chain: %d\n", chain );  R += tmp;
		sprintf( tmp, "MaxScore: %g\n", (double)score );  R += tmp;
		sprintf( tmp, "BlockNum: %d\n", getblocknum() );  R += tmp;

		for ( int i=0; i<bpos; i++ )
			R += "   ";
		sprintf( tmp, " %2d", bval );
		R += tmp;
		R += "\n";
	}

	//for ( int y=H-1; y>=0; y-- )
	for ( int y=H-T-1; y>=0; y-- )
	{
		for ( int x=0; x<W; x++ )
		{
			if ( F[y*W+x]==0 )
				R += "  .";
			else if ( F[y*W+x]>S )
				R += " ##";
			else
				sprintf( tmp, " %2d", F[y*W+x] ),
				R += tmp;
		}
		R += "\n";
	}
	return R;
}



//	�w��ʒu�Ƀp�b�N�𗎂Ƃ�
template<int W, int H, int T, int S, int N, int P>
void Field<W,H,T,S,N,P>::move( const char p[T*T], int xpos, int *chain )
{
	if ( step>=N )
	{
		step++;
		return;
	}

	//	����ۑ�
	memcpy( hist_F[step], F, sizeof F );
	hist_score[step] = score;
	hist_maxchain[step] = maxchain;
	hist_blocknum[step] = blocknum;
	memcpy( hist_height[step], height, sizeof height );

	if ( score<=-INF )
	{
		step++;
		return;
	}

	//	���C��
	//	updateb[l]!=LINF�Ȃ�΁A
	//	updateb[l]<=,<updatee[l]�̃u���b�N���ω����Ă���
	//	updatef[l]!=LINF�Ȃ��
	//	updatef[l]�ȏ�ɋ󔒂�����
	const int LINF = 9999;
	int updateb[linenum];
	int updatee[linenum];
	int	updatef[W];
	for ( int l=0; l<linenum; l++ )
		updateb[l] = +LINF,
		updatee[l] = -LINF;
	for ( int l=0; l<W; l++ )
		updatef[l] = LINF;

	//	�u��
	for ( int y=0; y<T; y++ )
	for ( int x=0; x<T; x++ )
	if ( p[y*T+x] > 0 )
	{
		int t = height[x+xpos]*W+(x+xpos);
		F[t] = p[y*T+x];
		if ( F[t]<=S )
			blocknum++;
		height[x+xpos]++;
		for ( int i=0; i<4; i++ )
		{
			int lu = pos2line[t][i];
			updateb[lu] = min( updateb[lu], t );
			updatee[lu] = max( updatee[lu], t+line[lu][2] );
		}
	}

	for ( int C=1; ; C++ )
	{
		//	�폜
		int E = 0;
		bool Fe[W*H] = {false};		//	�폜����u���b�N

		//	�l��S�̃u���b�N���폜
		if ( C==1 )
		{
			for ( int l=0; l<W; l++ )
			if ( updateb[l]!=LINF )
			{
				for ( int p=updateb[l]; p!=updatee[l]; p+=line[l][2] )
				if ( F[p]==S )
				{
					F[p] = 0;
					E++;
					updatef[l] = min( updatef[l], p );
					blocknum--;
				}
			}
		}

		//	���̑��̃u���b�N���폜
		for ( int l=0; l<linenum; l++ )
		if ( updateb[l]!=LINF )
		{
			int b = line[l][0];
			int e = line[l][1];
			int d = line[l][2];

			//	�J�n�ʒu��updateb[l]����߂��č��v��S�ȉ��̏�
			int p1 = updateb[l];
			int s = 0;
			while ( p1!=b && s<S )
				s += F[p1],
				p1 -= d;
			int p2 = p1;

			while ( p2!=e && p1!=updatee[l] )
			{
				//	�u���b�N������Ƃ���܂Ői�߂�
				while ( p2!=e && F[p2]==0 )
					p2 += d;
				if ( p2==e )
					break;
				p1 = p2;

				//	���Ⴍ�Ƃ�@
				int s = 0;
				while ( p2!=e && p1!=updatee[l] && F[p2]>0 || s>S )
				{
					if ( s<S )
					{
						s += F[p2];
						p2 += d;
					}
					else
					{
						s -= F[p1];
						p1 += d;
					}

					if ( s==S )
					{
						for ( int p=p1; p!=p2; p+=d )
						{
							Fe[p] = true;
							E++;
							int lu = pos2line[p][0];
							updatef[lu] = min( updatef[lu], p );
						}
					}
				}
			}

			updateb[l] = +LINF;
			updatee[l] = -LINF;
		}

		//	�����u���b�N��������΁A�A�������L�^���ďI��
		if ( E==0 )
		{
			maxchain = max( maxchain, C-1 );
			if ( chain!=NULL )
				*chain = C-1;
			break;
		}

		//	���ۂɃu���b�N������
		for ( int x=0; x<W; x++ )
		if ( updatef[x]!=LINF )
		{
			int e = height[x]*W+x;
			for ( int p=updatef[x]; p!=e; p+=line[x][2] )
			if ( Fe[p] )
			{
				F[p] = 0;
				blocknum--;
			}
		}

		//	�X�R�A�X�V
		int n = (step+1)/100;
		score += (1LL<<(min(C,P+n)-1)) * max(1,C-(P+n)+1) * E;

		//	����
		for ( int l=0; l<W; l++ )
		if ( updatef[l]!=LINF )
		{
			int p1 = updatef[l];
			int d = line[l][2];
			int e = height[l]*W+l;
			int p2 = p1;

			while ( true )
			{
				while ( p2!=e && F[p2]==0 )
					p2 += d;
				if ( p2==e )
					break;

				F[p1] = F[p2];
				F[p2] = 0;
				for ( int i=0; i<4; i++ )
				{
					int lu = pos2line[p1][i];
					updateb[lu] = min( updateb[lu], p1 );
					updatee[lu] = max( updatee[lu], p1+line[lu][2] );
				}
				p1 += d;
			}

			height[l] = p1/W;

			updatef[l] = LINF;
		}
	}

	//	�Q�[���I�[�o�[�`�F�b�N
	for ( int x=0; x<W; x++ )
		if ( height[x]>H-T )
			score = -INF;

	step++;
}



//	�����I�C���X�^���X��

#pragma warning(disable:4661)	// �����I�ȃe���v���[�g�̃C���X�^���X�����v���ɑ΂��ēK�؂Ȓ�`������܂���B
template class Field<  5,  8+2, 2, 10,   10, 25>;		//	Test
template class Field< 10, 16+4, 4, 10, 1000, 25>;		//	Small
template class Field< 15, 23+4, 4, 20, 1000, 30>;		//	Medium
template class Field< 20, 36+5, 5, 30, 1000, 35>;		//	Large
