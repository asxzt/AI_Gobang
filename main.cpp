#include <iostream>
#include <random>
using namespace std;

enum PieceType
{
    None,
    Black,
    White,
    Error
};
enum AIType
{
    RandomAI,
    ScoreGreedy,
    DoubleGreedy,
    RangeDP
};
struct Point
{
    int x,y;
};
bool IsNormalPiece(PieceType _pt)
{
    if(_pt==PieceType::Black)
        return 1;
    if(_pt==PieceType::White)
        return 1;
    return 0;
}
struct ChessBoard
{
public:
    ChessBoard()
    {}
    ~ChessBoard()
    {}
    void Set(int x,int y,PieceType _cbp)
    {
        if(x>16||x<1||y>16||y<1)return;
        _cb[x-1][y-1]=_cbp;
    }
    PieceType Get(int x,int y)
    {
        if(x>16||x<1||y>16||y<1)return PieceType::Error;
        return _cb[x-1][y-1];
    }
    PieceType Get(Point t)
    {
        if(t.x>16||t.x<1||t.y>16||t.y<1)return PieceType::Error;
        return _cb[t.x-1][t.y-1];
    }
private:
    PieceType _cb[15][15];
};
class AIChesser
{
public:
    AIChesser()
        :DefaultAIT(AIType::ScoreGreedy),RandomSeed(0),_mt(RandomSeed)
    {}
    AIChesser(int seed)
        :DefaultAIT(AIType::ScoreGreedy),RandomSeed(seed),_mt(RandomSeed)
    {}
    AIChesser(AIType _dait,int seed)
        :DefaultAIT(_dait),RandomSeed(seed),_mt(RandomSeed)
    {}
    ~AIChesser()
    {}
    Point AutoSet(ChessBoard _cb,PieceType _pt)
    {
        return AutoSet(_cb,_pt,DefaultAIT);
    }
    Point AutoSet(ChessBoard _cb,PieceType _pt,AIType _ait)
    {
        switch(_ait)
        {
            case AIType::RandomAI:return RandomSeter(_cb,_pt);break;
            case AIType::ScoreGreedy:return ScoreSeter(_cb,_pt);break;
            case AIType::DoubleGreedy:return ScoreSeter(_cb,_pt);break;
            case AIType::RangeDP:return ScoreSeter(_cb,_pt);break;
        }
        return Point({-1,-1});
    }

private:
    Point RandomSeter(ChessBoard _cb,PieceType _pt);
    Point ScoreSeter(ChessBoard _cb,PieceType _pt);
private:
    int GetSetScore(ChessBoard _cb,Point _set,PieceType _pt)
    {
        if(_pt==PieceType::Error||_pt==PieceType::None)return -1;
        //have a l

        _cb.Set(_set.x,_set.y,_pt);
        int fan[8][2]={{1,0},{-1,0},{0,1},{0,-1},{-1,-1},{-1,1},{1,-1},{1,1}};
        int total_score=0,t=0;
        //PieceType _fpt=(_pt==PieceType::Black?PieceType::White:PieceType::Black);
        for(int iii=0;iii<8;iii++)
        {
            t=0;
            Point now;
            now.x=_set.x+fan[iii][0];now.y=_set.y+fan[iii][1];
            PieceType now_t=_cb.Get(now);
            if(now_t!=PieceType::Black&&now_t!=PieceType::White)continue;
            while(_cb.Get(now)==now_t)
            {
                t++;
                now.x+=fan[iii][0];now.y+=fan[iii][1];
            }
            if(now_t==_pt)t++;
            if(now_t==_pt)
            {
                switch(t)
                {
                    case 2:total_score+=1;break;
                    case 3:total_score+=5;break;
                    case 4:total_score+=15;break;
                    case 5:total_score+=9999;break;
                }
            }
            if(now_t!=_pt)
            {
                switch(t)
                {
                    case 2:total_score+=1;break;
                    case 3:total_score+=6;break;
                    case 4:total_score+=16;break;
                    case 5:total_score+=999;break;
                }
            }
        }
        return total_score;
    }

    AIType DefaultAIT;
    int RandomSeed;
    std::mt19937 _mt;
    //rules
    bool RandWhenNoChoice=true;
    bool AlwaySetCenterWhenStart=true;
};
Point AIChesser::RandomSeter(ChessBoard _cb,PieceType _pt)
{
    for(int i=0;i<9999;i++)
    {
        int x=_mt()%15+1,y=_mt()%15+1;
        if(!IsNormalPiece(_cb.Get(x,y)))
        {
            return Point({x,y});
        }
    }
    return Point({-1,-1});
}
Point AIChesser::ScoreSeter(ChessBoard _cb,PieceType _pt)
{
    Point to_set({-1,-1});
    if(AlwaySetCenterWhenStart)
    {
        if(!IsNormalPiece(_cb.Get(to_set)))
        {
            to_set.x=7;
            to_set.y=7;
        }
    }
    int maxs=0;
    for(int i=1;i<=15;i++)
    {
        for(int j=1;j<=15;j++)
        {
            if(!IsNormalPiece(_cb.Get(i,j)))
            {
                if(maxs<GetSetScore(_cb,Point({i,j}),_pt))
                {
                    maxs=GetSetScore(_cb,Point({i,j}),_pt);
                    to_set=Point({i,j});
                }
                //if()
            }
        }
    }
    if(to_set.x==-1&&RandWhenNoChoice)
    {
        return RandomSeter(_cb,_pt);
    }
    else if(to_set.x==-1)
    {
        return Point({-1,-1});
    }
    return to_set;
}
PieceType WhoWin(ChessBoard _cb)
{
    PieceType winner=PieceType::None;
    int fan[8][2]={{1,0},{-1,0},{0,1},{0,-1},{-1,-1},{-1,1},{1,-1},{1,1}};
    for(int iii=1;iii<=15;iii++)
    {
        for(int jjj=1;jjj<=15;jjj++)
        {
            PieceType now=_cb.Get(iii,jjj);
            for(int ii=0;ii<8;ii++)
            {
                int i;
                for(i=0;i<=5;i++)
                {
                    Point nt({(iii+fan[ii][0]*i),(jjj+fan[ii][0]*i)});
                    if(_cb.Get(nt)!=now)break;
                }
                if(i==5)
                    return now;
            }
        }
    }
   return winner;
}
int main()
{
    
    ChessBoard _cb;
    AIChesser ra(AIType::RandomAI,34),sb(73706);
    Point set({5,5});
    while(set.x!=-1)
    {
        set=ra.AutoSet(_cb,PieceType::Black);
        cout<<"Black set on ("<<set.x<<","<<set.y<<")!"<<endl;
        if(set.x==-1)break;
        _cb.Set(set.x,set.y,PieceType::Black);

        if(WhoWin(_cb)!=PieceType::None)
        {
            if(WhoWin(_cb)==PieceType::Black)
                cout<<"Black win!"<<endl;
            if(WhoWin(_cb)==PieceType::White)
                cout<<"White win!"<<endl;
            break;
        }

        set=sb.AutoSet(_cb,PieceType::White);
        cout<<"White set on ("<<set.x<<","<<set.y<<")!"<<endl;
        if(set.x==-1)break;
        _cb.Set(set.x,set.y,PieceType::White);

        if(WhoWin(_cb)!=PieceType::None)
        {
            if(WhoWin(_cb)==PieceType::Black)
                cout<<"Black win!"<<endl;
            if(WhoWin(_cb)==PieceType::White)
                cout<<"White win!"<<endl;
            break;
        }
    }
    return 0;
}
