void turn(char dir);

typedef struct 
{
	int difx;
	int dify;
	char orientation;
} position_t;

void turn_north(position_t* pos);
void turn_east(position_t* pos);


// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
