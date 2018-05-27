#include<iostream>
#include<vector>
int main() {
	//just do some tests
	using namespace std;
	enum EE{
		A,
		B,
		C,
		D
	};
	using std::vector;
	struct AB {
		int p;
		~AB() {
			cout << "\nfuck you\n";
		}
	}*a;
	struct CC {
		AB ab;
	};
	vector<AB*>ee;
	a = new AB{};
	a->p = 123;
	cout << a->p;
	ee.push_back(a);
	vector<AB*>::iterator i;
	for (i = ee.begin(); i != ee.end(); i++)
		delete *i;
	cout << a->p << endl;
	CC*c = new CC;
	delete c;

	getchar();
}