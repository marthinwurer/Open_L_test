int index(int x, int y, int dimension){
	return y * dimension + x;
}

__kernel void calc_normals(
	__global const float * a,
	__global float4 * out,
	int dimension,
	int max,
	float dx,
	float dz)
{
	int gid = get_global_id(0);
	if( gid < max ){
		// compute the x and y indexes
		int x = gid % dimension;
		int y = gid / dimension;
		
		// south index
		int southy = (y + 1) % dimension; 
		int southindex = index( x, southy, dimension);
		
		// east index
		int eastx = (x + 1) % dimension; 
		int eastindex = index( y, eastx, dimension);
		
		// load the values
		float south = a[southindex];
		float east =  a[eastindex];
		float current = a[gid];
		
		/*
		A surface normal for a triangle can be calculated by taking the vector cross
		product of two edges of that triangle. The order of the vertices used in the 
		calculation will affect the direction of the normal (in or out of the face 
		w.r.t. winding).

		So for a triangle p1, p2, p3, if the vector U = p2 - p1 and the vector 
		V = p3 - p1 then the normal N = U X V and can be calculated by:

		Nx = UyVz - UzVy

		Ny = UzVx - UxVz

		Nz = UxVy - UyVx
*/
		float4 current_point = (float4)(0.0f, current, 0.0f, 0.0f);
		float4 south_point = (float4)(0.0f, south, dz, 0.0f);
		float4 east_point = (float4)(dx, east, 0.0f, 0.0f);
		float4 z_vector = south_point - current_point;
		float4 x_vector = east_point - current_point;
		
		// store the cross product of those vectors
		float4 output = cross(z_vector, x_vector);
		out[gid] = normalize(output);
	}
}
		
		
		
		
		
	