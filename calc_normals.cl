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
		float north =  a[northindex] * dy;
		float south = a[southindex] * dy;
		float east =  a[eastindex] * dy;
		float west =  a[westindex] * dy;
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
		float3 c = (float3)(0.0f, current, 0.0f);
		float3 s = (float3)(0.0f, south, dz);
		float3 e = (float3)(dx, east, 0.0f);
		float3 w = (float3)(-dx, west, 0.0f);
		float3 n = (float3)(0.0f, north, -dz);

		float3 z_vector = s - c;
		float3 x_vector = e - c;
		
		// store the cross product of those vectors
		float3 se = cross(z_vector, x_vector);
		
		float3 U = e - c;
		float3 V = n - c;
		float3 ne = cross(U, V);
		
		U = V;
		V = w - c;
		float3 nw = cross(U, V);
		
		U = V;
		V = z_vector;
		float3 sw = cross(U, V);
		
		
		

		
		float3 output = nw + sw + ne + se;

		
		
		
		
		out[gid] = (float4)(normalize(output), 1.0f);
	}
}
		
		
		
		
		
	