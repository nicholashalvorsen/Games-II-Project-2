

#include "d3dUtil.h"
#include "Geometry.h"
#include "Vertex.h"
#include "Box.h"
#include "Object.h"

class JetPack : public Object {
	private: 
		Box boxes[6];
	public:
		void draw();
		JetPack(){
			
		}

}
