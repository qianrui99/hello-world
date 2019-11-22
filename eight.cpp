#include<iostream>
#include<string.h> 
#include<math.h>
using namespace std;
#define M 3
#define N 3
int cc=0;
int id_counter=0;
void print_matrix(int src[M][N])
{//��ӡ���ݹ����ľ���
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
{//����src�������ݵ�des����aΪ����
	int* des_ptr=(int*)des;
	int* src_ptr=(int*)src;
	for(int i=0;i<a;++i)
	{
		*(des_ptr+i)=*(src_ptr+i);
	}
	return true;
}
int cal_reverse_nums(int nums[M][N])
{//����������ż�ԣ����ڿ����жϸ���ʼ״̬�Ƿ��н�
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
    result &= 1;//��λ���㣬���result��ż��������bit״̬���һλһ����0����֮Ϊ1
    return result;
}

struct loc
{//��Ҫ���ڼ�¼0��λ��
	int x;
	int y;
};
class node
{//������㣬���ڴ洢�������ӽڵ㼰��gx hx
public:
	int matrix[M][N];//�洢��������״̬�ľ���
	int hx;//����������ֵ
	int gx;//���ﵱǰ��㸶���Ĵ��ۣ��������Ϊ�������ӵ�һ��������ɵ����ӽڵ������
	node *next;//������������������㣬���ڴ���
	int id;//ÿ����㶼��һ������id�����ڸ��׽�����
	int father;//��ǰ���ĸ��ڵ�id��ͨ������ϢѰ�ҵ���ǰ���ĸ����
	node()
	{//Ĭ�ϳ�ʼ����ֻ��ͷ����õ��������������Ϊͷ�ڵ㲻��Ҫ�洢�κΰ�����״̬��ֻ��������������
		id = id_counter;
		father = -1;
		memset(matrix,-1,M*N);
		hx = -1;
		next = NULL;
	}
	node(int m[M][N],int hx_,int gx_,int id_,int father_)
	{//ÿһ���°������״̬����øó�ʼ������
		this->id = id_;
		this->father = father_;
		copy_matrix(matrix,m,M*N);
		this->hx = hx_;
		this->gx = gx_;
		next = NULL;
	}
};

class matrix
{//��������
private:
	int table_src[M][N];//size M*N 	//�洢���״̬�ľ���
	int table_des[M][N];			//�洢Ŀ��״̬�������ڶԱ�
	int table_best_now[M][N];		//ÿ�������½���Ҫ��open�����ҵ�fx=gx+hx��С�Ľ��
		int id_matrix;	//��ǰ��ý���id
		int father;		//��ǰ��ý��ĸ���id
		int gx;			//��ǰ���Ĵ��ۣ�������
	int move_dire[4][2];//�����������  //ͨ����0�����߿հף����������+-����ʵ�����������ƶ�
	loc blank_loc;		//�洢0���հף���λ��
	node *open;			//open�����������ڴ洢�Ѿ����ɵ�δ����İ�����״̬
	node *closed;		//closed�����ڴ洢�Ѿ�������Ľ�㣨��������״̬��

	int cal_hx(int now[N][N])
	{//ͨ�����㵱ǰ����now��Ŀ�����Ĳ��غϸ�������N*N-�غϸ�����������hxֵ
		//������㲻�غϸ�����ԭ����Ϊÿ�δ���İ�����״̬fx=gx+hxҪ����С������Ҫ��hx��gx��ҪС
		//��hx�����غϸ�����ԽС˵��Խ�ӽ�Ŀ�꺯��
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
				          	hx=hx+abs(i-k)+abs(j-l);//���������پ���
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
	{//��ʼ���������洢Ŀ�����des�ͳ�ʼ����src
		cout<<"des matrix:"<<endl;
			print_matrix(des);
		cout<<"src matrix:"<<endl;
			print_matrix(src);
		int direction[4][2]={{1,0},{0,1},{-1,0},{0,-1}};//������󣬴��ݸ�move_dire�����ƶ�0���հף�
		for(int i=0;i<4;++i)
		{
			for(int j=0;j<2;++j)
			{
				this->move_dire[i][j] = direction[i][j];//������Ϣ������move dire
			}
		}
		copy_matrix(table_src,src,M*N);//�����ݵĳ�ʼ�����Ƹ����Ա
		copy_matrix(table_des,des,M*N);//���Ƹ����е�Ŀ�ľ���
		copy_matrix(table_best_now,src,M*N);//Ĭ�ϳ�ʼ����Ϊ��ǰ��õľ���
		this->get_blank_loc();	//����0��λ��
		open = new node();		//����open���ͷ��
		memset(open->matrix,-1,M*N); 
		closed = new node();	//����closed�����ͷ
		memset(closed->matrix,-1,M*N);
		//�ѵ�ǰ��ý�����close����
		node* new_node = new node(table_best_now,cal_hx(table_best_now),0,id_counter++,-1);
			this->id_matrix = new_node->id;
			this->father = new_node->father;
			this->gx = new_node->gx;
		closed->next = new_node;
	}
	~matrix()
	{//����������delete��new����open���closed���ڴ�
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
	{//ȡ�ÿհ׸��λ��
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
	{//�Ƚ�����matrix�Ƿ����
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
		{//�ĸ������ƶ�0���հף�������2-4���°�����״̬
			int new_matrix[M][N]; //�����洢�����ɵ��ӽڵ�
			this->get_blank_loc();//����0��λ��
			this->blank_loc.x += this->move_dire[i][0];//4��ѭ����ÿ�δ���һ���·���
			this->blank_loc.y += this->move_dire[i][1];
			if(this->blank_loc.x<0 || this->blank_loc.x>=N || this->blank_loc.y<0 || this->blank_loc.y>=M)
			{//�ƶ�֮��Խ��������ʲô��������׼����ʼ��һ��������ƶ�
				// this->blank_loc.x -= this->move_dire[i][0];
				// this->blank_loc.y -= this->move_dire[i][1];
			}
			else
			{
				//��Ϊ���ӽڵ㶼���ɵ�ǰbest�ƶ����ɣ�����best��ֵ��newmatrix��Ȼ���new matrixԪ��0�����ƶ�
				copy_matrix(new_matrix,table_best_now,M*N);
				int num = new_matrix[blank_loc.x][blank_loc.y];//0����һ��λ�õ�Ԫ�ػ��������ƶ�
				new_matrix[blank_loc.x-this->move_dire[i][0]][blank_loc.y-this->move_dire[i][1]] = num;
				new_matrix[blank_loc.x][blank_loc.y] = 0;
				//���open��close��������û������½ڵ�matrix ,����У��������û�������
				//����ƿ�����Աȶ���̫��
				node* tempSearch = open->next;//ָ��open���һ����Ч���
				bool exist_before=false;//�洢�Ƿ��Ѿ�����
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
				{//open�����Ѵ��ڣ�ֱ���˳�
					continue;
				}
				tempSearch = closed->next;//ָ��closed���һ����Ч���
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
				{//�Ѿ����ڣ�ֱ�Ӷ���
					continue;
				}
				++cc;//���ɵ��¾����������������
				//cout<<"newMatrix:"<<cc++<<endl;
				//print_matrix(new_matrix);
				//������matrix��hxֵ
				int new_hx = this->cal_hx(new_matrix);  //���ӽڵ�newmatrix����������hxֵ
				int new_gx =this->gx + 1;				//�����gxֵ
				//׼����open�����в����ӽڵ㣬��fx=gx+hxֵ��С����˳����룬�����ҵ���õ�fxֵ���
				node *ptr = open;
				if(ptr->next == NULL)
				{//����Ϊ��
					node* node1 = new node(new_matrix,new_hx,new_gx,id_counter++,this->id_matrix);
					ptr->next = node1;
				}
				else
				{//��������
					while(ptr->next!=NULL && ptr->next->hx + ptr->next->gx <= new_hx + new_gx )
					{//����
						ptr = ptr->next;
					}
					node* new_one = new node(new_matrix,new_hx,new_gx,id_counter++,this->id_matrix);
					new_one->next = ptr->next;
					ptr->next = new_one;
				}

			}
		}

		//open->next�ǵ�ǰ��ý�㣬���临�Ƹ�best���滻��������µ�best��ʼ��һ�ֵ������ӽ������
		node* best = open->next;
			this->id_matrix = best->id;
			this->gx = best->gx;
			this->father = best->father;
		copy_matrix(table_best_now,best->matrix,M*N);
		open->next = best->next;
		best->next = closed->next;
		closed->next = best;//�����ǲ���Ӧ��delete���������������best��
		return true;
	}
	void get_sequence()
	{//�ҵ���Ŀ���㣬ͨ��closed������fatherid��ǰѰ�ң������ɴӳ�ʼ��㵽Ŀ�����·��,��seq_head����
		node* seq_head = new node();
		node* ptr = closed;
		node* tmp;
		int seq_father = this->father;
		while(ptr != NULL)
		{
			if(ptr->id == seq_father)
			{//�ҵ�ptr�ĸ��׽��
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
		{//��ӡ����������·��
		//	cout<<"id: "<<ptr->id<<"  father:"<<ptr->father<<endl;
			cout<<"step:"<<step++<<endl;
			print_matrix(ptr->matrix);
			ptr = ptr->next;
		}
		cout<<"step:"<<step<<endl;
		print_matrix(this->table_best_now);//��ӡ���һ������Ŀ����
	
	}
	bool run()
	{//��Ҫ����
		if(cal_reverse_nums(this->table_des) != cal_reverse_nums(this->table_src))
		{//�����޽��ж�
			cout<<"no result"<<endl;
			return false;
		}
		while(1)
		{
			if(this->compare_matrix(this->table_best_now,this->table_des))
			{//�ж��Ƿ��ҵ�Ŀ��״̬
				cout<<"find target, search:"<<cc<<" times"<<endl;
				this->get_sequence();
				//print_matrix(this->table_best_now);
				return true;
			}
			//û�ҵ�������best���ĸ������ƶ����ӽ��
			this->get_children();
			// if(open->next == NULL) //����һ��ע�͵��ı����޽��жϣ���open����ˣ�һ���޽���ʼ״̬��open��ջỨ10�������ϲ�������
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
	int dest_matrix[M][N]={{1,2,3},{8,0,4},{7,6,5}};//Ŀ��״̬
	int src_matrix[M][N]={4,8,2,0,7,3,6,1,5};		//��ʼ״̬
	matrix demo(src_matrix,dest_matrix);
	demo.run();
	return 0;
}

