extern unsigned char DFA[256][256];

typedef struct{
    const unsigned char * sources;
    unsigned char destination;
    const unsigned char * transitions;
}transition;

void scan(unsigned char c);
void exportDfa(const char * path);
