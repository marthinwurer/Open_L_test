int index(int x, int y, int dimension){
	return y * dimension + x;
}

__kernel void action(
	__global const float * a,
	__global float * out,
	int dimension,
	int max)
{
	int gid = get_global_id(0);
	if( gid < max ){
		// compute the x and y indexes
		int x = gid % dimension;
		int y = gid / dimension;
		
		// get the northward ( y - 1) index, and check for looping out of bounds
		int northy = (y - 1 + dimension) % dimension; 
		int northindex = index( x, northy, dimension);
		
		// south index
		int southy = (y + 1) % dimension; 
		int southindex = index( x, southy, dimension);
		
		// west index
		int westx = (x - 1 + dimension) % dimension; 
		int westindex = index( westx, y, dimension);
		
		// east index
		int eastx = (x + 1) % dimension; 
		int eastindex = index( eastx, y, dimension);
		
		// load the values
		float north = a[northindex];
		float south = a[southindex];
		float east =  a[eastindex];
		float west =  a[westindex];
		float current = a[gid];
		
		// average them
		out[gid] = (north + south + east + west + (current))/5.0f;
	}
}
		
		
		
		
		
	