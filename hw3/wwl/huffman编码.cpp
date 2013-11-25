#include <iostream>
#include <string>
#include <queue>
#include <fstream>
using namespace std;

const int MAX_SIZE=256;
struct huffman_node;
class huffman;
typedef huffman_node* node_ptr;
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
          for(int i=0;i<MAX_SIZE;i++)
            node_array[i]=new huffman_node(i);
          root=0;
          out_file.open(out_file_name.c_str(),ios::out);
          this->in_file_name=in_file_name;
        }
        void create_node_array()
        {
          // in_file.clear();
          in_file.open(in_file_name.c_str(),ios::in);
          unsigned char c = 0;
          c=in_file.get();
          while(!in_file.eof())                 
          {
            // cout<<"!"<<endl;
            node_array[c]->freq++;
            c=in_file.get();
          }
          in_file.close();
          // while(in >> in_file_name)
          //    node_array.push_back(node_ptr *id,node_ptr *freq);
          //    in.close();
        }
        void create_pq()
        {
             for (int i = 0; i < MAX_SIZE; ++i)
             {
              if(node_array[i]->freq!=0)
                pq.push(node_array[i]);
             }
        }   
        void create_huffman_tree()
        {
          if(pq.size()==1)
          {
            root=new huffman_node(0);
            root->parent=0;
            root->left=pq.top();
            root->freq=root->left->freq;
            root->left->parent=root;
            pq.pop();
          }
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
              // root->id =pq.node_ptr;
              // root->freq = pq.vector<node_ptr>;
              // pq.pop();
              // root->right = pq.node_ptr;
              // pq.pop();
              // root->left = pq.node_ptr;
              // pq.pop();
              // }
            }
            root=pq.top();
            pq.pop();
          }
        }
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
        void calculate_huffman_codes()
        {
          if(root==0)
            return;
          else
          {
            string code="0";
            node_ptr p=root;
            while(1)
            {
              if(p->code=="")
              {
                p->code=code;
                if(p->left!=0)
                {
                  code+="0";
                  p=p->left;
                  continue;
                }
                if(p->right!=0)
                {
                  code+="1";
                  p=p->right;
                  continue;
                }
                else
                {
                  p=p->parent;
                  code.erase(code.length()-1);
                  continue;
                }
              }
              else
              {
                if(p->right->code=="")
                {
                  code+="1";
                  p=p->right;
                  continue;
                }
                else
                {
                  if(p->parent==0)
                    break;
                  p=p->parent;
                  code.erase(code.length()-1);
                  continue;
                }
              }
            }
          }
          for (int i = 0; i < MAX_SIZE; ++i)
          {
            if(node_array[i]->code!="")
              node_array[i]->code.erase(0,1);
          }
        }
        void save_to_file()
        {

          in_file.open(in_file_name.c_str(),ios::in);
          in_file.clear();
          unsigned char c = 0;
          c = in_file.get();
          string s;
          while(!in_file.eof())
          {
            s+=node_array[c]->code;
            c = in_file.get();
          }
          // cout<<s<<endl;
          int size=0;
          for (int i = 0; i < MAX_SIZE; ++i)
            if(node_array[i]->freq!=0)
              size++;
          out_file<<size<<" ";
          for (int i = 0; i < MAX_SIZE; ++i)
            if(node_array[i]->freq!=0)
              out_file<<node_array[i]->id<<" "<<node_array[i]->freq<<" ";
          unsigned int count=0;//记录编码最后一个字节中从右开始共有几个有效位
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
          out_file.close();
          // for (int i = 0; i < MAX_SIZE; ++i)
          // {
          //   cout << (int)node_array[i]->id << ": " << node_array[i]->id << " -- freq: "<<node_array[i]->freq <<" , code: " << node_array[i]->code << endl;
          // }
        }

};
class huffmanDecode :public huffman
{
public:
  huffmanDecode(string in_file_name,string out_file_name)
    :huffman(in_file_name,out_file_name)
  {}
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
    // for (int i = 0; i < MAX_SIZE; ++i)
    // {
    //   cout << (int)node_array[i]->id << ": " << node_array[i]->id << " -- freq: "<<node_array[i]->freq <<" , code: " << node_array[i]->code << endl;
    // }
  }
  void readCode()
  {
    unsigned char c=in_file.get();
    while(!in_file.eof())
    {
      string s;
      for (int i = 0; i < 8; ++i)
      {
        if(c&1)
        {
          s = '1'+s;
        }
        else
        {
          s = '0'+s;
        }
        c>>=1;
      }
      str += s;
      c=in_file.get();
    }
    // cout<<str<<endl;
    str.erase(str.length()-8,8);//舍弃最后的一个字节的内容，因为不是编码的主体
    // cout<<str<<endl;
    in_file.clear();
    in_file.seekg(-1,ios::end);
    c=in_file.get();
    int count=(unsigned int)c;//读取最后一个编码从右往左的有效位
    // cout<<count<<endl;
    if(count)//还原编码主体为字符串
    {
      string temp=str.substr(str.length()-8,8);
      str.erase(str.length()-8,8);
      str+=temp.substr(8-count,count);
    }
    // cout<<str<<endl;
    in_file.close();
  }
  void save_to_file()
  {
    string tran;
    // cout<<str<<endl;
    node_ptr p=root;
    // return ;
    int i=0;
    while(1)
    {
      p=root;
      if(i==str.length())
        break;
      while(1)
      {
        if(p->left==0)
        {
          // cout<<p->id<<endl;
          tran+=p->id;
          break;
        }
        if(str[i]=='0')
        {
          p=p->left; 
          i++;       
          continue;
        }
        if(str[i]=='1')
        {
          p=p->right;
          i++;
          continue;
        }
      }
    }
    out_file<<tran;
    out_file.close();
  }
private:
  string str;//huffman编码串
};

int main(int argc, char const *argv[])
{
  huffman h(argv[1],argv[2]);
  h.create_node_array();
  h.create_pq();
  h.create_huffman_tree();
  // h.preorder();
  h.calculate_huffman_codes();
  h.save_to_file();
  
  cout << "done!" << endl;
  return 0;
}
// int main(int argc, char const *argv[])
// {
//   huffmanDecode h(argv[1],argv[2]);
//   h.readMap();
//   h.create_pq();
//   h.create_huffman_tree();
//   // h.preorder();
//   h.readCode();
//   h.save_to_file();
//   return 0;
// }
