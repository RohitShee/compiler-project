#include<iostream>
#include<fstream>
#include<set>
using namespace std;
set<char> punctuationSet;
set<string> keywordSet, symbolTable, operatorSet;

string helpToIdentify(string &token){
        if(token[0]=='"') return "String";
		if(token[0]=='\''){
            if(token.size()>3) return "Invalid Token";
            return "Character";
        }
		bool isNumber = true;
		for(char c : token){
			if((c<'0' || c>'9') && c!='.' ) isNumber=false;
		}
        if(isNumber){
            if(token.find('.')!=string::npos) return "Float";
            return "Integer";
        }
		if(token[0]>='0' && token[0]<='9') return"Invalid Token";
        return "Identifier";
}

//Helper function to retrieve and store keywords in set, stored in "keywords.txt"
void buildKeywordSet() {
	ifstream fi("keywords.txt", ios::in);
	string str;
	if(fi.is_open())
		while(fi >> str)
			keywordSet.insert(str);
	fi.close();
}

//Helper function to retrieve and store punctuations in set, stored in "punctuations.txt"
void buildPunctuationSet() {
	ifstream fi("punctuations.txt", ios::in);
	char ch;
	if(fi.is_open())
		while(fi >> ch)
			punctuationSet.insert(ch);
	fi.close();
}

//Helper functions to retrieve and store operators in set, stored in "operators.txt"
void buildOperatorSet() {
	ifstream fi("operators.txt", ios::in);
	string ch;
	if(fi.is_open()) 
		while(fi >> ch) 
			operatorSet.insert(ch);
	fi.close();
}

//Retrieve each line from "input.c" input file, 
//and find if current character is either a operator or punctuation
//and check for keyword existence
void computeTokens() {
	ifstream fi("sample.c", ios::in);
	string str;
	int row = 1, col, startidx, endidx;
	string token, type;
	if(fi.is_open()) {
		cout<<"----------------------------------------------------------------------\n";
		printf("\tToken \t\tType \t\tRow \t\tColumn \n");
		cout<<"----------------------------------------------------------------------\n";
		while(getline(fi,str)) {
			startidx = -1;
           // cout<<str<<endl;
			for(int i=0;i<=str.size();i++) {
				if(i==str.size() || str[i]==' ') {
					if(startidx!=-1) {
						token = str.substr(startidx,endidx-startidx+1);
						col = startidx+1;
						if(keywordSet.find(token) != keywordSet.end()) {
							type = "keyword";
						}
						else {
							type = helpToIdentify(token);
							symbolTable.insert(token);
						}
						cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					}
					startidx=-1;
				}
				else if(str[i]=='\t') continue;
                else if(i+1<str.size() && str[i]=='/' && str[i+1]=='/') break;
                else if(i+1<str.size() && operatorSet.find(str.substr(i,2)) != operatorSet.end()){
                    if(startidx!=-1) {
						token = str.substr(startidx,endidx-startidx+1);
						col = startidx+1;
						if(keywordSet.find(token) != keywordSet.end()) {
							type = "keyword";
						}
						else {
							type = helpToIdentify(token);
							symbolTable.insert(token);
						}
						cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					}
					token = str.substr(i,2);
					col = i+2;
					type = "operator";
					cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					startidx=-1;
                    i++;
                }
				else if(operatorSet.find(str.substr(i,1)) != operatorSet.end()) {
					if(startidx!=-1) {
						token = str.substr(startidx,endidx-startidx+1);
						col = startidx+1;
						if(keywordSet.find(token) != keywordSet.end()) {
							type = "keyword";
						}
						else {
							type = helpToIdentify(token);
							symbolTable.insert(token);
						}
						cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					}
					token = str[i];
					col = i+1;
					type = "operator";
					cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					startidx=-1;
				}
				else if(punctuationSet.find(str[i]) != punctuationSet.end()) {
					if(startidx!=-1) {
						token = str.substr(startidx,endidx-startidx+1);
						col = startidx+1;
						if(keywordSet.find(token) != keywordSet.end()) {
							type = "keyword";
						}
						else {
							type = helpToIdentify(token);
							symbolTable.insert(token);
						}
						cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					}
					token = str[i];
					col = i+1;
					type = "punctuation";
					cout<<"\t"<<token<<"\t\t"<<type<<" \t"<<row<<"\t"<<"\t"<<col<<"\n";
					startidx=-1;
				}
				else if(startidx==-1) startidx=endidx=i;
				else endidx=i;
			}
			row++;
		}
		cout<<"----------------------------------------------------------------------\n";
	}
	fi.close();
}

int main() {
	buildKeywordSet();
	buildPunctuationSet();
	buildOperatorSet();
	computeTokens();
	

	keywordSet.clear();
	operatorSet.clear();
	punctuationSet.clear();
	return 0;
}