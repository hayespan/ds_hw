#include <iostream>
#include <string>
#include <queue>
#include <fstream>
using namespace std;
//���鳤��
const int MAX_SIZE=256;
//ǰ������
struct huffman_node;
class huffman;

typedef huffman_node* node_ptr;
//�������ڵ���
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
//������������
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
          // 256����ĸȫ�������ڴ�ռ�
          for(int i=0;i<MAX_SIZE;i++)
            node_array[i]=new huffman_node(i);
          root=0;
          out_file.open(out_file_name.c_str(),ios::out);
          this->in_file_name=in_file_name;
        }
        void create_node_array()
        {
          // ��һ�δ��ļ���ͳ��256����ĸƵ�ʣ�
          in_file.open(in_file_name.c_str(),ios::in);
          unsigned char c = 0;
          c=in_file.get();
          while(!in_file.eof())                 
          {
            node_array[c]->freq++;
            c=in_file.get();
          }
          // �ر��ļ�
          in_file.close();
        }
        // ��Ƶ�ʲ�Ϊ0����ĸ�ڵ�ѹ�����ȶ���
        void create_pq()
        {
          for (int i = 0; i < MAX_SIZE; ++i)
          {
            if(node_array[i]->freq!=0)
              pq.push(node_array[i]);
          }
        }
        // �������ȶ��д�����������
        void create_huffman_tree()
        {
          // ������ȶ���Ϊ�գ�ʲôҲ������
          // �������1��Ԫ�أ����ýڵ���Ϊ���ڵ����Ҷ��
          if(pq.size()==1)
          {
            root=new huffman_node(0);
            root->parent=0;
            root->left=pq.top();
            root->freq=root->left->freq;
            root->left->parent=root;
            pq.pop();
          }
          // ����д���1��Ԫ�أ���ÿ�������½ڵ㣬Ȼ��ȡ��ͷ2���ڵ���Ϊ
          // �½ڵ������Ҷ�ӣ������½ڵ��Ƶ�ʡ�����Ҷ�ӵĸ�ָ��
          // ���Ѹ��½ڵ����
          // ѭ����������ֱ�����н���1���ڵ�
          // ��rootָ��ýڵ�
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
        // ǰ����������ڲ���
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
        // ʹ��ѭ�����������������������нڵ㣨�ڽڵ㡢Ҷ�ӣ��Ĺ��������벢�����ڵ��code
        void calculate_huffman_codes()
        {
          // ���rootΪ0����������ʲô������
          if(root==0)
            return;
          // Ϊ���㴦��һ��ʼ�ı��봮��Ϊ"0"��������""���գ�
          // ���ø�ָ�����ѭ������
          else
          {
            string code="0";
            node_ptr p=root;// ����ָ��
            while(1)
            {
              // �����ǰδ����������õ�ǰ���봮���и�ֵ
              if(p->code=="")
              {
                p->code=code;
                // ������������������봮+"0"
                // Ȼ�󽫱���ָ��ָ��������������ѭ��
                if(p->left!=0)
                {
                  code+="0";
                  p=p->left;
                  continue;
                }
                // ������������������봮+"1"
                // Ȼ�󽫱���ָ��ָ��������������ѭ��
                if(p->right!=0)
                {
                  code+="1";
                  p=p->right;
                  continue;
                }
                // ���û�к��ӣ�˵���ýڵ���Ҷ��
                // �򽫱��봮ȥ�����һ������
                // ������ָ��ָ���ף�����ѭ��
                else
                {
                  p=p->parent;
                  code.erase(code.length()-1);
                  continue;
                }
              }
              // ����ýڵ��ѱ��������˵����ڵ������Һ��ӣ����������϶��������
              // ����������һ���������
              else
              {
                // ���������û������������봮+"1"
                // ������ָ��ָ��������������ѭ��
                if(p->right->code=="")
                {
                  code+="1";
                  p=p->right;
                  continue;
                }
                // ���������Ҳ�����������ô˵�����Ե�ǰ�ڵ�Ϊ�������ѱ����
                else
                {
                  // ����ýڵ�ĸ�����0����ýڵ��Ǹ��ڵ㣬soѭ������
                  if(p->parent==0)
                    break;
                  // ����ýڵ㲻Ϊ���ڵ㣬�����ĸ��׵����������ܻ�û������
                  // ���Ա���ָ��ָ�����ĸ���
                  // Ȼ����봮ȥ�����һ���ַ�������ѭ��
                  p=p->parent;
                  code.erase(code.length()-1);
                  continue;
                }
              }
            }
          }
          // �����й��������붼ȥ���ʼ��"0"
          // ��"0"Ϊ�˷��㴦����ӵ�~why?��
          // ���һ��ʼ��""����ѭ��һ��ʼ�����ڵ��code�Իᱻ��Ϊ""
          // ��Ϊ�жϵ�ǰ�ڵ��Ƿ��Ѿ���������ͨ���ж���code�Ƿ�Ϊ��
          // ����ѭ���е��Ӹ��ڵ�����������ص����ڵ�ʱ����������ѭ��
          for (int i = 0; i < MAX_SIZE; ++i)
          {
            if(node_array[i]->code!="")
              node_array[i]->code.erase(0,1);
          }
        }
        // ����ӳ��Ե���Ŀ��ӳ��ԣ�������Ϣ
        void save_to_file()
        {
          in_file.open(in_file_name.c_str(),ios::in);
          in_file.clear();
          unsigned char c = 0;
          c = in_file.get();
          string s;// ���ɱ���-c++string����
          while(!in_file.eof())
          {
            s+=node_array[c]->code;
            c = in_file.get();
          }
          int size=0;// ͳ��ӳ��Ե���Ŀ
          for (int i = 0; i < MAX_SIZE; ++i)
            if(node_array[i]->freq!=0)
              size++;
          out_file<<size<<" ";// ���ӳ��Ե���Ŀ
          for (int i = 0; i < MAX_SIZE; ++i)// �������Ƶ�ʷ�0��ӳ�䣨��ĸ-Ƶ�ʣ�
            if(node_array[i]->freq!=0)
              out_file<<node_array[i]->id<<" "<<node_array[i]->freq<<" ";
          // ����������c++string��ı��봮��ÿ8λΪһ��unsigned char��������ļ�
          // ��Ϊλ���������8�ı�������Ȼ���봮���ļ�λ�Ի���һ���ֽ���
          // ���ǻ�༸��û�õ�λ
          // �������Ӧ���һ���ֽ�����¼�������һ���ֽ��д��ҿ�ʼ���м�����Чλ
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
          out_file<<(unsigned char)count;//������count
          // �ر��ļ�
          out_file.close();
        }

};
class huffmanDecode :public huffman
{
private:
  string str;//huffman�������崮
public:
  huffmanDecode(string in_file_name,string out_file_name)
    :huffman(in_file_name,out_file_name)
  {}
  // ��ȡӳ���
  void readMap()
  {
    int size=0;
    in_file.open(in_file_name.c_str(),ios::in);
    in_file>>size;//��ȡӳ����
    unsigned char c=0;
    unsigned char temp=in_file.get();//��ȡӳ������ߵ����ÿո�
    unsigned int f=0;
    while(size--)
    {
      c = in_file.get();//��ȡĿ���ַ�
      temp=in_file.get();//��ȡĿ���ַ�������ÿո�
      in_file>>f;//��ȡƵ��
      temp=in_file.get();//��ȡƵ�ʺ�����ÿո�
      node_array[c]->freq=f;//��ȡ��ĸ-Ƶ�ʲ����node_array
    }
  }
  // ��ȡ����
  void readCode()
  {
    // ��ȡ���б�����Ϣ������������������һ���ֽڣ���¼��Чλ��count����
    // ͬʱת��Ϊc++ string
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
    //��������һ���ֽ�(count)����Ϊ���Ǳ��������
    str.erase(str.length()-8,8);
    // �ƶ��ļ���ָ�룬������ȡ���һ��byte(count)
    in_file.clear();
    in_file.seekg(-1,ios::end);
    c=in_file.get();
    int count=(unsigned int)c;
    //��ԭ��������Ϊ�ַ���
    // ���count����0����˵��������������һ���ֽ���Ҫȥ������λ��������ƴ��
    // ���countΪ0��������������账��
    if(count)
    {
      string temp=str.substr(str.length()-8,8);
      str.erase(str.length()-8,8);
      str+=temp.substr(8-count,count);
    }
    // �ر��ļ�
    in_file.close();
  }
  void save_to_file()
  {
    string tran;// ���봮
    node_ptr p=root;// ����ָ��
    int i=0;
    // �����������崮�͹����������н���
    while(1)
    {
      // �Ӹ��ڵ㿪ʼ
      p=root;
      // ����Ե���ĩβ��һλ����������
      if(i==str.length())
        break;
      // �����������
      while(1)
      {
        // ���������Ϊ0����˵���ýڵ���Ҷ��
        // �����ַ��ӵ����봮ĩβ������ѭ�����Ӹ��ڵ㿪ʼ
        if(p->left==0)
        {
          tran+=p->id;
          break;
        }
        // �����ǰ�ַ�Ϊ0��������������
        if(str[i]=='0')
        {
          p=p->left; 
          i++;       
          continue;
        }
        // �����ǰ�ַ�Ϊ1��������������
        if(str[i]=='1')
        {
          p=p->right;
          i++;
          continue;
        }
      }
    }
    out_file<<tran;// ������봮
    out_file.close();
  }
};

int main(int argc, char const *argv[])
{
  huffman h(argv[1],argv[2]);
  cout<<"�����ַ����飬ͳ��Ƶ��..."<<endl;
  h.create_node_array();
  cout<<"�������ȶ���..."<<endl;
  h.create_pq();
  cout<<"������������..."<<endl;
  h.create_huffman_tree();
  // h.preorder();
  cout<<"�������������..."<<endl;
  h.calculate_huffman_codes();
  cout<<"��ʼд�����ļ�..."<<endl;
  h.save_to_file();
  cout<<"������ɣ�"<<endl;
  return 0;
}
// int main(int argc, char const *argv[])
// {
//   huffmanDecode h(argv[1],argv[2]);
//   cout<<"��ȡӳ�����Ϣ..."<<endl;
//   h.readMap();
//   cout<<"�������ȶ���..."<<endl;
//   h.create_pq();
//   cout<<"������������..."<<endl;
//   h.create_huffman_tree();
//   // h.preorder();
//   cout<<"��ȡ����..."<<endl;
//   h.readCode();
//   cout<<"��ʼд�����ļ�..."<<endl;
//   h.save_to_file();
//   cout<<"������ɣ�"<<endl;
//   return 0;
// }
