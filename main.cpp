#include<iostream>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<dir.h>

using namespace std;

int compare(const char *a,const char *b)
{
	int x=0,y=0,i=0;
	
	if(strlen(a)==strlen(b))
	{
		for(i=0;i<strlen(a);i++)
		{
			x=a[i];
			y=b[i];
			
			if((x-y)%32!=0)
			{
				return 0;
			}
		}
	}
	
	else
	{
		return 0;
	}
	
	return 1;
}

class directory
{
	DIR *p;
	directory *parent;
	string name,address;
	
	public:
		directory();
		directory(directory *,string);
		void display();
		directory *open(string);
		directory *up();
		void search(string,string);
		int check();
		string get_address()
		{
			return address;
		}
};

directory::directory()
{
	p=opendir("/");
	address="/";
	parent=NULL;
	name="Home";
}

void directory::search(string r_address,string x)
{
	DIR *z=opendir(r_address.c_str());
	if(z==NULL)
	{
		return;
	}
	
	struct dirent *a=readdir(z);
	
	while(a!=NULL)
	{
		string name;
		name=(a->d_name);
		
		if(name.find(x,0)<strlen(name.c_str()))
		{
			cout<<"File name:"<<name<<endl;
			cout<<"Address:"<<r_address+name<<endl<<endl;
		}
		
		if(a->d_name[0]!='$'&&a->d_name[0]!='.')
		{	
			search(r_address+name+"/",x);
		}
		
		a=readdir(z);
	}
	
	return;
}

directory::directory(directory *par,string x)
{
	address=par->address+x+"/";
	p=opendir(address.c_str());
	parent=par;
	name=x;
}

directory *directory::open(string x)
{
	struct dirent *c=readdir(p);
	
	while(c!=NULL)
	{
		if(compare(x.c_str(),c->d_name))
		{
			directory *ret=new directory(this,c->d_name);
			p=opendir(address.c_str());
			
			return ret;
		}
		c=readdir(p);
	}
	
	cout<<"----------------------------------------------------------------------"<<endl;
	cout<<"\tFolder does not exist"<<endl;
	cout<<"----------------------------------------------------------------------"<<endl;
	
	return NULL;
}

directory *directory::up()
{
	return parent;
}

void directory::display()
{
	p=opendir(address.c_str());
	struct dirent *c=readdir(p);
	
	cout<<"----------------------------------------------------------------------"<<endl;
	cout<<"\t\t"<<name<<endl;
	cout<<"----------------------------------------------------------------------"<<endl;
	
	while(c!=NULL)
	{
		cout<<c->d_name<<endl;
		c=readdir(p);
	}
	
	p=opendir(address.c_str());
}

int directory::check()
{
	if(p==NULL)
	{
		return 1;
	}
	
	return 0;
}

struct node
{
	directory *ptr;
	struct node *up;
	struct node *down;
};

class stack
{
	struct node *root,*top;
	
	public:
		stack()
		{
			top=NULL;
			root=NULL;
		}
		void push(directory *);
		directory *pop();
		void empty();
};

void stack::empty()
{
	root=NULL;
	top=NULL;
}

void stack::push(directory *item)
{
	if(root==NULL)
	{
		root=new struct node;
		root->ptr=item;
		root->up=NULL;
		root->down=NULL;
		top=root;
		
		return;
	}
	
	else
	{
		struct node *cur,*temp;
		temp=new struct node;
		cur=root;
		while(cur->up!=NULL)
		{
			cur=cur->up;
		}
		temp->down=cur;
		temp->up=NULL;
		temp->ptr=item;
		cur->up=temp;
		top=temp;
		
		return;
	}
}
	
directory *stack::pop()
{
	if(root==NULL)
	{
		return NULL;
	}
	
	struct node *temp;
	directory *ret;
	
	if(top==root)
	{
		temp=top;
		top=NULL;
		ret=temp->ptr;
		delete root;
		root=NULL;
		
		return ret;
	}
		
	temp=top;
	top=top->down;
	top->up=NULL;
	ret=temp->ptr;
	
	return ret;
}

main()
{
	directory root,*cur=&root,*prev=NULL;
	root.display();
	char comm[5];
	string name;
	stack back,front;
	
	while(1)
	{
		cout<<"----------------------------------------------------------------------"<<endl;
		cout<<"Enter command:";
		cin>>comm;
		
		if(compare(comm,"open"))
		{
			cout<<"Enter folder name:";
			cin.sync();
			getline(cin,name);
			prev=cur;
			cur=cur->open(name);
			back.push(prev);
			
			if(cur==NULL)
			{
				cur=back.pop();
			}
			
			if(cur->check())
			{
				cout<<"----------------------------------------------------------------------"<<endl;
				cout<<"\t\tAccess Denied"<<endl;
				cur=back.pop();
			}
			
			else
			{
				front.empty();
				cur->display();
			}
		}
		
		else if(compare(comm,"back"))
		{
			prev=cur;
			front.push(prev);
			cur=back.pop();
	
			if(cur==NULL)
			{
				cur=prev;
			}
			
			cur->display();
		}
		
		else if(compare(comm,"front"))
		{
			prev=cur;
			back.push(prev);
			cur=front.pop();
			
			if(cur==NULL)
			{
				cout<<"NULL"<<endl;
				cur=prev;
			}
			
			cur->display();
		}
		
		else if(compare(comm,"up"))
		{
			prev=cur;
			cur=cur->up();
			back.push(prev);
			
			if(cur==NULL)
			{
				cur=back.pop();
			}
			
			cur->display();
			front.empty();
		}
		
		else if(compare(comm,"home"))
		{
			cur=&root;
			cur->display();
		}
		
		else if(compare(comm,"search"))
		{
			string key;
			
			cout<<"Enter keyword:";
			cin.sync();
			getline(cin,key);
			cout<<"----------------------------------------------------------------------"<<endl;
			cout<<"Search results:"<<endl<<endl;
			cur->search(cur->get_address(),key);
		}
		
		else if(compare(comm,"create"))
		{
			int x;
			
			cout<<"Enter folder name:";
			cin.sync();
			getline(cin,name);
			
			x=mkdir((cur->get_address()+name+"/").c_str());
			
			if(x)
			{
				cout<<"Cannot create folder"<<endl;
			}
			
			else
			{
				cout<<"Folder created"<<endl;
			}
			
			cur->display();
		}
		
		else if(compare(comm,"delete"))
		{
			int x;
			
			cout<<"Enter folder name:";
			cin.sync();
			getline(cin,name);
			
			x=rmdir((cur->get_address()+name+"/").c_str());
			
			if(x)
			{
				cout<<"Cannot delete folder"<<endl;
			}
			
			else
			{
				cout<<"Folder deleted"<<endl;
			}
			
			cur->display();
		}
		
		else if(compare(comm,"rename"))
		{
			string new_name;
			int x;
			
			cout<<"Enter folder name:";
			cin.sync();
			getline(cin,name);
			
			cout<<"Enter new name for folder:";
			cin>>new_name;
			
			x=rename((cur->get_address()+name+"/").c_str(),(cur->get_address()+new_name+"/").c_str());
			
			if(x)
			{
				cout<<"Cannot rename folder"<<endl;
			}
			
			else
			{
				cout<<"Folder renamed"<<endl;
			}
			
			cur->display();
		}
		
		else if(compare(comm,"exit"))
		{
			cout<<"----------------------------------------------------------------------"<<endl;
			exit(0);
		}
	}
	
	return 0;
}
