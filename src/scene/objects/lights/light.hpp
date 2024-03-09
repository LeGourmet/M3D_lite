#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "utils/define.hpp"
#include "scene/objects/object.hpp"

#include <string>

namespace M3D
{
	enum LIGHT_TYPE { POINT, SPOT, DIRECTIONAL };

	namespace Scene
	{
		class Light : public Object
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			Light(LIGHT_TYPE p_type, const Vec3f& p_color, const float p_intensity, const float p_innerConeAngle=-1.f, const float p_outerConeAngle=-1.f) 
				: _type(p_type), _color(p_color), _intensity(p_intensity/683.f) {
				_cosInnerConeAngle = glm::cos(p_innerConeAngle);
				_cosOuterConeAngle = glm::cos(p_outerConeAngle); 
				_range = ((p_type==LIGHT_TYPE::DIRECTIONAL) ? FLT_MAX : (float)glm::sqrt(256.f*_intensity*glm::max<float>(_color.x,glm::max<float>(_color.y,_color.z))));
			}

			~Light(){}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			inline LIGHT_TYPE	getType()		const { return _type; }

			inline const Vec3f& getColor()		const { return _color; }
			inline		 float	getIntensity()	const { return _intensity; }
			inline const Vec3f	getEmissivity() const { return _color*_intensity; }
			inline		 float	getRange()		const { return _range; }

			inline float getCosInnerConeAngle() const { return _cosInnerConeAngle; }
			inline float getCosOuterConeAngle() const { return _cosOuterConeAngle; }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			LIGHT_TYPE _type;

			Vec3f _color = VEC3F_ONE;
			float _intensity = 0.f;
			float _range = 0.f;

			float _cosInnerConeAngle = -1.f;
			float _cosOuterConeAngle = -1.f;
		};
	}
}

#endif
