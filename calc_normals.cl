int index(int x, int y, int dimension){
	return y * dimension + x;
}

__kernel void calc_normals(
	__global const float * a,
	__global float4 * out,
	int dimension,
	int max,
	float dx,
	float dy,
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
		int eastindex = index( eastx, y, dimension);
		
		// load the values
		float south = a[southindex] * dy;
		float east =  a[eastindex] * dy;
		float current = a[gid] * dy;
		
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
		float3 current_point = (float3)(0.0f, current, 0.0f);
		float3 south_point = (float3)(0.0f, south, dz);
		float3 east_point = (float3)(dx, east, 0.0f);
		float3 z_vector = south_point - current_point;
		float3 x_vector = east_point - current_point;
		
		// store the cross product of those vectors
		float3 output = cross(z_vector, x_vector);
		out[gid] = (float4)(normalize(output), 1.0f);
	}
}
		
		
		
		
		
	