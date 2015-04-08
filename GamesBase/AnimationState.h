#ifndef __Animation_State_
#define __Animation_State_
#include <vector>
#include "constants.h"
#include "d3dUtil.h"

struct Animation {
	Vector3 Srotate, Stranslate, Sscale, Erotate, Etranslate, Escale;
};

class AnimationState {
public:
	void addAnimation(Vector3 Stranslate, Vector3 Srotate, Vector3 Sscale, Vector3 Etranslate, Vector3 Erotate, Vector3 Escale) {
		Animation a;
		a.Srotate = Srotate;
		a.Stranslate = Stranslate;
		a.Sscale = Sscale;
		a.Erotate = Erotate;
		a.Etranslate = Etranslate;
		a.Escale = Escale;
		animations.push_back(a);
	}

	Animation getAnimation(int i) {
		return animations[i];
	}
private:
	std::vector<Animation> animations;
};
#endif