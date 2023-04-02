#ifndef __SCENE_GRAPH_NODE_HPP__
#define __SCENE_GRAPH_NODE_HPP__

#include "glm/detail/_fixes.hpp"
#include "glm/gtx/quaternion.hpp"

#include "utils/define.hpp"

namespace M3D
{
    namespace Scene
    {
        class SceneGraphNode
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            SceneGraphNode(SceneGraphNode* p_parent, const Vec3f& p_translation, const Vec3f& p_scale, const Quatf& p_rotation) :
                _parent(p_parent), _translation(p_translation), _scale(p_scale), _rotation(p_rotation) {
                _updateLocalTransformation();
            }
            ~SceneGraphNode() {}

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            inline const SceneGraphNode* getParent() const { return _parent; }
            inline const Quatf& getRotation() const { return _rotation; }
            inline const Vec3f& getScale() const { return _scale; }
            inline const Vec3f& getTranslation() const { return _translation; }
            inline const Mat4f& getTransformation() const { return _transformation; }

            // ----------------------------------------------------- SETTERS -------------------------------------------------------
            void setRotation(const Quatf& p_rotation) {
                _rotation = p_rotation;             // CHECK normalize
                _updateLocalTransformation();
            }

            void setScaling(const Vec3f& p_scale) {
                _scale = p_scale;                   // CHECK negative
                _updateLocalTransformation();
            }

            void setTranslation(const Vec3f& p_translation) {
                _translation = p_translation;
                _updateLocalTransformation();
            }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------        
            const Mat4f computeTransformation() const {
                Mat4f result = _transformation;
                SceneGraphNode* current = _parent;

                while (current != nullptr) result = current->_transformation * result;

                return result;
            }

            void translate(const Vec3f& p_delta) {
                _translation += p_delta;
                _updateLocalTransformation();
            }

            void rotate(const Vec3f& p_delta) { 
                _rotation *= Quatf(p_delta);    // CHECK normalize
                _updateLocalTransformation(); 
            }

            void scale(const Vec3f& p_delta) {
                _scale *= p_delta;              // CHECK negative
                _updateLocalTransformation(); 
            }

            void rotateAround(const Vec3f& p_pivot, const Vec3f& p_delta) {
                Vec3f position = computeTransformation()*Vec4f(VEC3F_ZERO,1.);
                translate(p_pivot + Quatf(p_delta) * (position - p_pivot) - position);
            }

            void lookAt(const Vec3f& p_target) {
                /*Vec3f forwardVector = glm::normalize(p_lookAt - _position);

                _rotation *= glm::normalize(
                    Quatf(glm::dot(_front, forwardVector) + 1.f, 
                          glm::cross(_front, forwardVector)));*/
            }

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            SceneGraphNode* _parent = nullptr;
            
            Quatf _rotation = QUATF_ID;
            Vec3f _scale = VEC3F_ONE;
            Vec3f _translation = VEC3F_ZERO;
            
            Mat4f _transformation = MAT4F_ID;

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void _updateLocalTransformation() {
                _transformation = glm::translate(MAT4F_ID, _translation)
                                * glm::mat4_cast(_rotation)
                                * glm::scale(MAT4F_ID, _scale);
            }
        };
    }
}

#endif