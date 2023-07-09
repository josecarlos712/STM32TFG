void initTest();
void cleanQueue();
void test();
void loadInstructions();
void loadInstructionsChunk(char* instructions, uint8_t* insSize);
void separateInstructions(const char* instructions, int* array1, int* array2, int* numInstructions);
int sendIntrucctionstoQueue(int* insArray, int* durArray, const int numInstructions);
void printStates();
