#include <cstdlib>
#include <cstring>
#include <unistd.h>

class N {
public:
	char annotation[100];
	int  number;

	N(int number) {
		this->number = number;
	}

	virtual int operator+(N &arg) {
		return this->number + arg.number;
	}

	virtual void setAnnotation(char *str) {
		size_t len;

		len = strlen(str);
		memcpy(this->annotation, str, len);
		return;
	}
};

int main(int ac, char **av) {
	N *nclass_1;
	N *nclass_2;

	if (ac < 2) {
		_exit(1);
	}

	nclass_1 = new N(5); // ptr stocke a esp+0x1c
	nclass_2 = new N(6); // ptr stocke a esp+0x18

	nclass_1->setAnnotation(av[1]);

	return (*nclass_2) + (*nclass_1);
}
