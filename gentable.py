# coding: utf-8
# テーブルを生成

def pspin(W,H,T,S,N,P):
	print "const int Field<%d,%d,%d,%d,%d,%d>::pspin[4][%d] = "%(W,H+T,T,S,N,P,T*T)
	print "{"
	P = [[0]*T for t in range(T)]
	i = 0
	for y in range(T):
		for x in range(T):
			P[y][x] = i
			i += 1
	for r in range(4):
		s = ""
		i = 0
		for y in range(T):
			for x in range(T):
				s += "%d,"%P[y][x]
				i += 1
		print "    {"+s+"},"
		
		B = P
		P = [[0]*T for t in range(T)]
		for y in range(T):
			for x in range(T):
				P[y][x] = B[x][T-y-1]
	print "};"

def line(W,H,T,S,N,P):
	print "const int Field<%d,%d,%d,%d,%d,%d>::line[%d][4] = "%(W,H+T,T,S,N,P,3*(W+H+T)-2)
	print "{"
	# 縦
	s = ""
	for x in range(W):
		s += "{%d,%d,%d},"%(x,W*(H+T)+x,W)
	print "    "+s
	# 横
	s = ""
	for y in range(H+T):
		s += "{%d,%d,%d},"%(y*W,y*W+W,1)
	print "    "+s
	# 左下-右上
	s = ""
	for x in range(W)[::-1]:
		s += "{%d,%d,%d},"%(x,x+(W-x)*(W+1),W+1)
	for y in range(1,H+T):
		s += "{%d,%d,%d},"%(y*W,y*W+min(W,H+T-y)*(W+1),W+1)
	print "    "+s
	# 右下-左上
	s = ""
	for x in range(W):
		s += "{%d,%d,%d},"%(x,x+(x+1)*(W-1),W-1)
	for y in range(1,H+T):
		s += "{%d,%d,%d},"%(y*W+W-1,y*W+W-1+min(W,H+T-y)*(W-1),W-1)
	print "    "+s
	print "};"

def pos2line(W,H,T,S,N,P):
	L = []
	# 縦
	for x in range(W):
		L += [(x,W*(H+T)+x,W)]
	# 横
	for y in range(H+T):
		L += [(y*W,y*W+W,1)]
	# 左下-右上
	for x in range(W)[::-1]:
		L += [(x,x+(W-x)*(W+1),W+1)]
	for y in range(1,H+T):
		L += [(y*W,y*W+min(W,H+T-y)*(W+1),W+1)]
	# 右下-左上
	for x in range(W):
		L += [(x,x+(x+1)*(W-1),W-1)]
	for y in range(1,H+T):
		L += [(y*W+W-1,y*W+W-1+min(W,H+T-y)*(W-1),W-1)]
	
	F = [[] for i in range(W*(H+T))]
	for i,l in enumerate(L):
		for p in range(*l):
			F[p] += [i]
	
	print "const int Field<%d,%d,%d,%d,%d,%d>::pos2line[%d][4] = "%(W,H+T,T,S,N,P,W*(H+T))
	print "{"
	for y in range(H+T):
		s = ""
		for f in F[y*W:(y+1)*W]:
			s += "{%d,%d,%d,%d},"%tuple(f)
		print "    "+s
	print "};"

#          W,  H, T,  S,    N,  P
test	=  5,  8, 2, 10,   10, 25
small	= 10, 16, 4, 10, 1000, 25
medium	= 15, 23, 4, 20, 1000, 30
large	= 20, 36, 5, 30, 1000, 35

pspin(*test)
pspin(*small)
pspin(*medium)
pspin(*large)

line(*test)
line(*small)
line(*medium)
line(*large)

pos2line(*test)
pos2line(*small)
pos2line(*medium)
pos2line(*large)
