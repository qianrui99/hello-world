#include<iostream>
#include<string.h> 
#include<math.h>
using namespace std;
#define M 3
#define N 3
int cc=0;
int id_counter=0;
void print_matrix(int src[M][N])
{//打印传递过来的矩阵
	for(int i=0;i<M;++i)
	{
		for(int j=0;j<N;++j)
		{
			cout<<src[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"--------------"<<endl;
}
bool copy_matrix(int des[][N],int src[][N],int a)
{//复制src矩阵内容到des矩阵，a为数量
	int* des_ptr=(int*)des;
	int* src_ptr=(int*)src;
	for(int i=0;i<a;++i)
	{
		*(des_ptr+i)=*(src_ptr+i);
	}
	return true;
}
int cal_reverse_nums(int nums[M][N])
{//计算逆序奇偶性，用于快速判断该起始状态是否有解
    int result = 0;
    int nums_[M*N-1];
    int ptr=0;
    for(int i=0;i<M;++i)
    {
        for(int j=0;j<N;++j)
        {
			if(nums[i][j] == 0)
			{
				continue;
			}
            nums_[ptr++] = nums[i][j];
        }
    }
    for(int i=1;i<M*N;++i)
    {
        int local_sum=0;
        for(int j=0;j<i;++j)
        {
            if(nums_[i]<nums_[j])
                ++local_sum;
        }
        result += local_sum;
    }
    result &= 1;//做位运算，如果result是偶数则它的bit状态最后一位一定是0，反之为1
    return result;
}

struct loc
{//主要用于记录0的位置
	int x;
	int y;
};
class node
{//单个结点，用于存储新生成子节点及其gx hx
public:
	int matrix[M][N];//存储八数码新状态的矩阵
	int hx;//启发函数的值
	int gx;//到达当前结点付出的代价（这里代码为层数，从第一个结点生成到此子节点层数）
	node *next;//链表，用于链接其他结点，便于处理
	int id;//每个结点都有一个自身id，用于父亲结点检索
	int father;//当前结点的父节点id，通过此信息寻找到当前结点的父结点
	node()
	{//默认初始化，只有头结点用到了这个函数，因为头节点不需要存储任何八数码状态，只是用来保持链表
		id = id_counter;
		father = -1;
		memset(matrix,-1,M*N);
		hx = -1;
		next = NULL;
	}
	node(int m[M][N],int hx_,int gx_,int id_,int father_)
	{//每一个新八数码的状态会调用该初始化函数
		this->id = id_;
		this->father = father_;
		copy_matrix(matrix,m,M*N);
		this->hx = hx_;
		this->gx = gx_;
		next = NULL;
	}
};

class matrix
{//八数码类
private:
	int table_src[M][N];//size M*N 	//存储最初状态的矩阵
	int table_des[M][N];			//存储目标状态矩阵，用于对比
	int table_best_now[M][N];		//每次生成新结点后要从open表中找到fx=gx+hx最小的结点
		int id_matrix;	//当前最好结点的id
		int father;		//当前最好结点的父亲id
		int gx;			//当前结点的代价（层数）
	int move_dire[4][2];//下右上左遍历  //通过对0（或者空白）对坐标进行+-操作实现上下左右移动
	loc blank_loc;		//存储0（空白）的位置
	node *open;			//open表（链表），用于存储已经生成但未处理的八数码状态
	node *closed;		//closed表，用于存储已经处理过的结点（即八数码状态）

	int cal_hx(int now[N][N])
	{//通过计算当前矩阵now和目标矩阵的不重合个数（即N*N-重合个数）来计算hx值
		//这里计算不重合个数的原因，因为每次处理的八数码状态fx=gx+hx要求最小，所以要求hx和gx都要小
		//而hx代表不重合个数，越小说明越接近目标函数
		int hx = 0;
		bool test = true; 
		for(int i=0;i<N;++i)
		{
			for(int j=0;j<N;++j)
			{
				test = true;
				for(int k=0;k<N;k++)
				{
					for(int l=0;l<N;l++)
					{
						if((i!=k||j!=l)&&(this->table_des[i][j] == now[k][l]))
			        	{
				          	hx=hx+abs(i-k)+abs(j-l);//计算曼哈顿距离
							test=false; 
				        }
				        if(test==false) break;
					}
					if(test==false) break;
				}
				
			}
		}
		return hx;
	}

public:
	matrix(int src[][N],int des[][N])
	{//初始化函数，存储目标矩阵des和初始矩阵src
		cout<<"des matrix:"<<endl;
			print_matrix(des);
		cout<<"src matrix:"<<endl;
			print_matrix(src);
		int direction[4][2]={{1,0},{0,1},{-1,0},{0,-1}};//方向矩阵，传递给move_dire用于移动0（空白）
		for(int i=0;i<4;++i)
		{
			for(int j=0;j<2;++j)
			{
				this->move_dire[i][j] = direction[i][j];//方向信息拷贝给move dire
			}
		}
		copy_matrix(table_src,src,M*N);//将传递的初始矩阵复制给类成员
		copy_matrix(table_des,des,M*N);//复制给类中的目的矩阵
		copy_matrix(table_best_now,src,M*N);//默认初始矩阵为当前最好的矩阵
		this->get_blank_loc();	//更新0的位置
		open = new node();		//生成open表表头，
		memset(open->matrix,-1,M*N); 
		closed = new node();	//生成closed链表表头
		memset(closed->matrix,-1,M*N);
		//把当前最好结点加入close表中
		node* new_node = new node(table_best_now,cal_hx(table_best_now),0,id_counter++,-1);
			this->id_matrix = new_node->id;
			this->father = new_node->father;
			this->gx = new_node->gx;
		closed->next = new_node;
	}
	~matrix()
	{//析构函数，delete掉new出来open表和closed表内存
		node*ptr = open;
		while(ptr != NULL)
		{
			node *to_del=ptr;
			ptr = ptr->next;
			delete to_del;
			to_del = NULL;
		}
		ptr = closed;
		while(ptr != NULL)
		{
			node *to_del=ptr;
			ptr = ptr->next;
			delete to_del;
			to_del = NULL;
		}
	}
	void get_blank_loc()
	{//取得空白格的位置
		for(int i=0;i<M;++i)
		{
			for(int j=0;j<N;++j)
			{
				if(this->table_best_now[i][j]==0)
				{
					this->blank_loc.x=i;
					this->blank_loc.y=j;
					return;
				}
			}
		}
	}
	bool compare_matrix(int src_mat[M][N],int des_mat[M][N])
	{//比较两个matrix是否相等
		for(int i=0;i<M;++i)
		{
			for(int j=0;j<N;++j)
			{
				if(src_mat[i][j] != des_mat[i][j])
				{
					return false;
				}

			}
		}
		return true;
	}
	bool get_children()
	{
		for(int i=0;i<4;++i)
		{//四个方向移动0（空白），生成2-4个新八数码状态
			int new_matrix[M][N]; //用来存储新生成的子节点
			this->get_blank_loc();//更新0的位置
			this->blank_loc.x += this->move_dire[i][0];//4次循环，每次代表一个新方向
			this->blank_loc.y += this->move_dire[i][1];
			if(this->blank_loc.x<0 || this->blank_loc.x>=N || this->blank_loc.y<0 || this->blank_loc.y>=M)
			{//移动之后越界的情况，什么都不做，准备开始下一个方向的移动
				// this->blank_loc.x -= this->move_dire[i][0];
				// this->blank_loc.y -= this->move_dire[i][1];
			}
			else
			{
				//因为新子节点都是由当前best移动生成，所以best赋值给newmatrix，然后对new matrix元素0进行移动
				copy_matrix(new_matrix,table_best_now,M*N);
				int num = new_matrix[blank_loc.x][blank_loc.y];//0和下一步位置的元素互换，即移动
				new_matrix[blank_loc.x-this->move_dire[i][0]][blank_loc.y-this->move_dire[i][1]] = num;
				new_matrix[blank_loc.x][blank_loc.y] = 0;
				//检查open和close链表中有没有这个新节点matrix ,如果有，舍弃这项，没有则继续
				//性能瓶颈：对比队列太长
				node* tempSearch = open->next;//指向open表第一个有效结点
				bool exist_before=false;//存储是否已经存在
				while(tempSearch!=NULL)
				{
					if(this->compare_matrix(new_matrix,tempSearch->matrix))
					{
						exist_before = true;
						break;
					}		
					tempSearch = tempSearch->next;
				}
				if(exist_before == true)
				{//open表中已存在，直接退出
					continue;
				}
				tempSearch = closed->next;//指向closed表第一个有效结点
				while(tempSearch!=NULL)
				{
					if(this->compare_matrix(new_matrix,tempSearch->matrix))
					{
							exist_before = true;
							break;
					}
					tempSearch = tempSearch->next;
				}
				if(exist_before == true)
				{//已经存在，直接丢弃
					continue;
				}
				++cc;//生成的新矩阵计数器，调试用
				//cout<<"newMatrix:"<<cc++<<endl;
				//print_matrix(new_matrix);
				//计算新matrix的hx值
				int new_hx = this->cal_hx(new_matrix);  //新子节点newmatrix的启发函数hx值
				int new_gx =this->gx + 1;				//其层数gx值
				//准备向open链表中插入子节点，按fx=gx+hx值从小到大顺序插入，便于找到最好的fx值结点
				node *ptr = open;
				if(ptr->next == NULL)
				{//链表为空
					node* node1 = new node(new_matrix,new_hx,new_gx,id_counter++,this->id_matrix);
					ptr->next = node1;
				}
				else
				{//插入排序
					while(ptr->next!=NULL && ptr->next->hx + ptr->next->gx <= new_hx + new_gx )
					{//插入
						ptr = ptr->next;
					}
					node* new_one = new node(new_matrix,new_hx,new_gx,id_counter++,this->id_matrix);
					new_one->next = ptr->next;
					ptr->next = new_one;
				}

			}
		}

		//open->next是当前最好结点，将其复制给best并替换，用这个新的best开始下一轮的生成子结点任务
		node* best = open->next;
			this->id_matrix = best->id;
			this->gx = best->gx;
			this->father = best->father;
		copy_matrix(table_best_now,best->matrix,M*N);
		open->next = best->next;
		best->next = closed->next;
		closed->next = best;//这里是不是应该delete掉这个独立出来的best？
		return true;
	}
	void get_sequence()
	{//找到了目标结点，通过closed表，根据fatherid向前寻找，并生成从初始结点到目标结点的路径,即seq_head链表
		node* seq_head = new node();
		node* ptr = closed;
		node* tmp;
		int seq_father = this->father;
		while(ptr != NULL)
		{
			if(ptr->id == seq_father)
			{//找到ptr的父亲结点
				//ptr = ptr->next;
				tmp = new node(ptr->matrix,ptr->hx,ptr->gx,ptr->id,ptr->father);
				tmp->next = seq_head->next;
				seq_head->next = tmp;
				seq_father = ptr->father;

				//ptr = closed->next;
			}
			else
			{
				ptr = ptr->next;
			}
		}
		ptr = seq_head->next;
		int step=0;
		while(ptr != NULL)
		{//打印完整的生成路线
		//	cout<<"id: "<<ptr->id<<"  father:"<<ptr->father<<endl;
			cout<<"step:"<<step++<<endl;
			print_matrix(ptr->matrix);
			ptr = ptr->next;
		}
		cout<<"step:"<<step<<endl;
		print_matrix(this->table_best_now);//打印最后一个，即目标结点
	
	}
	bool run()
	{//主要函数
		if(cal_reverse_nums(this->table_des) != cal_reverse_nums(this->table_src))
		{//快速无解判断
			cout<<"no result"<<endl;
			return false;
		}
		while(1)
		{
			if(this->compare_matrix(this->table_best_now,this->table_des))
			{//判断是否找到目标状态
				cout<<"find target, search:"<<cc<<" times"<<endl;
				this->get_sequence();
				//print_matrix(this->table_best_now);
				return true;
			}
			//没找到就生成best的四个方向移动的子结点
			this->get_children();
			// if(open->next == NULL) //这是一个注释掉的暴力无解判断，即open表空了，一般无解起始状态到open表空会花10分钟以上才能跑完
			// {
			// 	cout<<"no result"<<endl;
			// 	return false;
			// }
		}
		return false;
	}

};

int main()
{
/* 
dest matrix
1,2,3
8,0,4
7,6,5
source matrix
4,8,2 5 2 8
0,7,3 3 0 1
6,1,5 6 7 4
*/
	int dest_matrix[M][N]={{1,2,3},{8,0,4},{7,6,5}};//目的状态
	int src_matrix[M][N]={4,8,2,0,7,3,6,1,5};		//初始状态
	matrix demo(src_matrix,dest_matrix);
	demo.run();
	return 0;
}

