#include <iostream>
#include <string>
#include <queue>
#include <fstream>
using namespace std;
//数组长度
const int MAX_SIZE=256;
//前向声明
struct huffman_node;
class huffman;

typedef huffman_node* node_ptr;
//哈夫曼节点类
struct huffman_node
{
  huffman_node(int c)
  {
    id=(unsigned char)c;
    freq=0;
    code="";
    left=0;
    right=0;
    parent=0;
  }
  unsigned char id;
  int freq;
  string code;
  node_ptr left,
           right,
           parent;
};
//哈夫曼编码类
class huffman
{
  protected:
      node_ptr node_array[MAX_SIZE];
      fstream in_file,
              out_file;
      node_ptr root;
      string in_file_name;
      class compare
      {
          public:
             bool operator()( const node_ptr& c1,
                              const node_ptr& c2) const
             {return(*c1).freq > (*c2).freq;}
      };
      priority_queue< node_ptr, vector<node_ptr>, compare > pq;

  public:
        huffman(string in_file_name,string out_file_name)
        {
          // 256个字母全部申请内存空间
          for(int i=0;i<MAX_SIZE;i++)
            node_array[i]=new huffman_node(i);
          root=0;
          out_file.open(out_file_name.c_str(),ios::out);
          this->in_file_name=in_file_name;
        }
        void create_node_array()
        {
          // 第一次打开文件，统计256个字母频率，
          in_file.open(in_file_name.c_str(),ios::in);
          unsigned char c = 0;
          c=in_file.get();
          while(!in_file.eof())                 
          {
            node_array[c]->freq++;
            c=in_file.get();
          }
          // 关闭文件
          in_file.close();
        }
        // 把频率不为0的字母节点压入优先队列
        void create_pq()
        {
          for (int i = 0; i < MAX_SIZE; ++i)
          {
            if(node_array[i]->freq!=0)
              pq.push(node_array[i]);
          }
        }
        // 借助优先队列创建哈夫曼树
        void create_huffman_tree()
        {
          // 如果优先队列为空，什么也不用做
          // 如果仅有1个元素，即该节点作为根节点的左叶子
          if(pq.size()==1)
          {
            root=new huffman_node(0);
            root->parent=0;
            root->left=pq.top();
            root->freq=root->left->freq;
            root->left->parent=root;
            pq.pop();
          }
          // 如果有大于1个元素，则每次申请新节点，然后取队头2个节点作为
          // 新节点的左右叶子，更新新节点的频率、左右叶子的父指针
          // 最后把该新节点入队
          // 循环上述过程直到队列仅有1个节点
          // 将root指向该节点
          else if(pq.size()>1)
          {
            while(pq.size()!=1)
            {
              node_ptr p=new huffman_node(0);
              p->left=pq.top();
              pq.pop();
              p->right=pq.top();
              pq.pop();
              p->freq=p->left->freq+p->right->freq;
              p->left->parent=p;
              p->right->parent=p;
              pq.push(p);
            }
            root=pq.top();
            pq.pop();
          }
        }
        // 前序遍历，用于测试
        void preorder()
        {
          pre(root);
        }
        void pre(node_ptr root)
        {
          if(root==0)
            return;
          else
          {
            cout<<(int)root->id<<" "<<root->id<<" freq: "<<root->freq << " parent: " <<root->parent<<endl;
            pre(root->left);
            pre(root->right);
          }
        }
        // 使用循环遍历哈夫曼树，计算所有节点（内节点、叶子）的哈夫曼编码并赋给节点的code
        void calculate_huffman_codes()
        {
          // 如果root为0，空树，则什么都不做
          if(root==0)
            return;
          // 为方便处理将一开始的编码串设为"0"，而不是""（空）
          // 利用父指针完成循环遍历
          else
          {
            string code="0";
            node_ptr p=root;// 遍历指针
            while(1)
            {
              // 如果当前未编码过，则用当前编码串进行赋值
              if(p->code=="")
              {
                p->code=code;
                // 如果有左子树，而编码串+"0"
                // 然后将遍历指针指向左子树，继续循环
                if(p->left!=0)
                {
                  code+="0";
                  p=p->left;
                  continue;
                }
                // 如果有右子树，而编码串+"1"
                // 然后将遍历指针指向右子树，继续循环
                if(p->right!=0)
                {
                  code+="1";
                  p=p->right;
                  continue;
                }
                // 如果没有孩子，说明该节点是叶子
                // 则将编码串去除最后一个符号
                // 将遍历指针指向父亲，继续循环
                else
                {
                  p=p->parent;
                  code.erase(code.length()-1);
                  continue;
                }
              }
              // 如果该节点已被编码过，说明这节点有左右孩子，且左子树肯定被编码过
              // 但右子树不一定被编码过
              else
              {
                // 如果右子树没被编码过，编码串+"1"
                // 将遍历指针指向右子树，继续循环
                if(p->right->code=="")
                {
                  code+="1";
                  p=p->right;
                  continue;
                }
                // 如果右子树也被编码过，那么说明该以当前节点为根的树已编码过
                else
                {
                  // 如果该节点的父亲是0，则该节点是根节点，so循环结束
                  if(p->parent==0)
                    break;
                  // 如果该节点不为根节点，则它的父亲的右子树可能还没被编码
                  // 所以遍历指针指向它的父亲
                  // 然后编码串去除最后一个字符，继续循环
                  p=p->parent;
                  code.erase(code.length()-1);
                  continue;
                }
              }
            }
          }
          // 将所有哈夫曼编码都去掉最开始的"0"
          // （"0"为了方便处理而加的~why?）
          // 如果一开始是""，则循环一开始，根节点的code仍会被赋为""
          // 因为判断当前节点是否已经被编码是通过判断其code是否为空
          // 所以循环中当从根节点的左子树返回到根节点时，将陷入死循环
          for (int i = 0; i < MAX_SIZE; ++i)
          {
            if(node_array[i]->code!="")
              node_array[i]->code.erase(0,1);
          }
        }
        // 保存映射对的数目，映射对，编码信息
        void save_to_file()
        {
          in_file.open(in_file_name.c_str(),ios::in);
          in_file.clear();
          unsigned char c = 0;
          c = in_file.get();
          string s;// 生成编码-c++string对象
          while(!in_file.eof())
          {
            s+=node_array[c]->code;
            c = in_file.get();
          }
          int size=0;// 统计映射对的数目
          for (int i = 0; i < MAX_SIZE; ++i)
            if(node_array[i]->freq!=0)
              size++;
          out_file<<size<<" ";// 输出映射对的数目
          for (int i = 0; i < MAX_SIZE; ++i)// 输出所有频率非0的映射（字母-频率）
            if(node_array[i]->freq!=0)
              out_file<<node_array[i]->id<<" "<<node_array[i]->freq<<" ";
          // 接下来，将c++string类的编码串以每8位为一个unsigned char存入编码文件
          // 因为位数如果不是8的倍数，虽然编码串最后的几位仍会存进一个字节里
          // 但是会多几个没用的位
          // 所以最后应多加一个字节来记录编码最后一个字节中从右开始共有几个有效位
          unsigned int count=0;
          c=0;
          for (int i = 0; i < s.length(); ++i)
          {
            if(s[i]=='1')
              c++;
            if(count==7)
            {
              out_file<<c;
              c=0;
              count=0;
              continue;
            }
            c<<=1;
            count++;
          }
          if(count!=0)
          {
            c>>=1;
            out_file<<c;
          }
          out_file<<(unsigned char)count;//最后输出count
          // 关闭文件
          out_file.close();
        }

};
class huffmanDecode :public huffman
{
private:
  string str;//huffman编码主体串
public:
  huffmanDecode(string in_file_name,string out_file_name)
    :huffman(in_file_name,out_file_name)
  {}
  // 读取映射对
  void readMap()
  {
    int size=0;
    in_file.open(in_file_name.c_str(),ios::in);
    in_file>>size;//读取映射数
    unsigned char c=0;
    unsigned char temp=in_file.get();//读取映射数后边的无用空格
    unsigned int f=0;
    while(size--)
    {
      c = in_file.get();//读取目标字符
      temp=in_file.get();//读取目标字符后的无用空格
      in_file>>f;//读取频率
      temp=in_file.get();//读取频率后的无用空格
      node_array[c]->freq=f;//读取字母-频率并存进node_array
    }
  }
  // 读取编码
  void readCode()
  {
    // 读取所有编码信息（包含编码主体和最后一个字节（记录有效位的count））
    // 同时转化为c++ string
    unsigned char c=in_file.get();
    while(!in_file.eof())
    {
      string s;
      for (int i = 0; i < 8; ++i)
      {
        if(c&1)
          s = '1'+s;
        else
          s = '0'+s;
        c>>=1;
      }
      str += s;
      c=in_file.get();
    }
    //舍弃最后的一个字节(count)，因为不是编码的主体
    str.erase(str.length()-8,8);
    // 移动文件流指针，单独读取最后一个byte(count)
    in_file.clear();
    in_file.seekg(-1,ios::end);
    c=in_file.get();
    int count=(unsigned int)c;
    //还原编码主体为字符串
    // 如果count不是0，则说明编码主体的最后一个字节需要去除无用位，并进行拼接
    // 如果count为0，则编码主体无需处理
    if(count)
    {
      string temp=str.substr(str.length()-8,8);
      str.erase(str.length()-8,8);
      str+=temp.substr(8-count,count);
    }
    // 关闭文件
    in_file.close();
  }
  void save_to_file()
  {
    string tran;// 解码串
    node_ptr p=root;// 遍历指针
    int i=0;
    // 遍历编码主体串和哈夫曼树进行解码
    while(1)
    {
      // 从根节点开始
      p=root;
      // 如果以到达末尾下一位，则解码结束
      if(i==str.length())
        break;
      // 否则继续解码
      while(1)
      {
        // 如果左子树为0，则说明该节点是叶子
        // 则将其字符加到解码串末尾，跳出循环，从根节点开始
        if(p->left==0)
        {
          tran+=p->id;
          break;
        }
        // 如果当前字符为0，则向左子树走
        if(str[i]=='0')
        {
          p=p->left; 
          i++;       
          continue;
        }
        // 如果当前字符为1，则向右子树走
        if(str[i]=='1')
        {
          p=p->right;
          i++;
          continue;
        }
      }
    }
    out_file<<tran;// 输出解码串
    out_file.close();
  }
};

int main(int argc, char const *argv[])
{
  huffman h(argv[1],argv[2]);
  cout<<"创建字符数组，统计频率..."<<endl;
  h.create_node_array();
  cout<<"创建优先队列..."<<endl;
  h.create_pq();
  cout<<"创建哈夫曼树..."<<endl;
  h.create_huffman_tree();
  // h.preorder();
  cout<<"计算哈夫曼编码..."<<endl;
  h.calculate_huffman_codes();
  cout<<"开始写编码文件..."<<endl;
  h.save_to_file();
  cout<<"编码完成！"<<endl;
  return 0;
}
// int main(int argc, char const *argv[])
// {
//   huffmanDecode h(argv[1],argv[2]);
//   cout<<"读取映射对信息..."<<endl;
//   h.readMap();
//   cout<<"创建优先队列..."<<endl;
//   h.create_pq();
//   cout<<"创建哈夫曼树..."<<endl;
//   h.create_huffman_tree();
//   // h.preorder();
//   cout<<"读取编码..."<<endl;
//   h.readCode();
//   cout<<"开始写解码文件..."<<endl;
//   h.save_to_file();
//   cout<<"解码完成！"<<endl;
//   return 0;
// }
