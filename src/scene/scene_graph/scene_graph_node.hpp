#ifndef __SCENE_GRAPH_NODE_HPP__
#define __SCENE_GRAPH_NODE_HPP__

#include "glm/detail/_fixes.hpp"
#include "glm/gtx/quaternion.hpp"

#include "utils/define.hpp"

#include <vector>

namespace M3D
{
    namespace Scene
    {
        class SceneGraphNode
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            SceneGraphNode(const Vec3f& p_translation, const Vec3f& p_scale, const Quatf& p_rotation) :
                _translation(p_translation), _scale(p_scale), _rotation(p_rotation) {
                update();
            }

            ~SceneGraphNode() {}

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline const SceneGraphNode* getParent() const { return _parent; }
            inline const std::vector<SceneGraphNode*> getChilds() const { return _childs; }

            inline const Vec3f getPosition() const { return _transformation * Vec4f( VEC3F_ZERO, 1.); }
            inline const Vec3f getFront()    const { return _transformation * Vec4f(-VEC3F_Z, 0.); } // gltf use z as front
            inline const Vec3f getBack()     const { return _transformation * Vec4f( VEC3F_Z, 0.); } // gltf use -z as back
            inline const Vec3f getUp()       const { return _transformation * Vec4f( VEC3F_Y, 0.); }
            inline const Vec3f getDown()     const { return _transformation * Vec4f(-VEC3F_Y, 0.); }
            inline const Vec3f getRight()    const { return _transformation * Vec4f( VEC3F_X, 0.); } // gltf use -X as right !?
            inline const Vec3f getLeft()     const { return _transformation * Vec4f(-VEC3F_X, 0.); } // gltf use -X as right !?
            
            inline const Quatf& getRotation() const { return _rotation; }
            inline const Vec3f& getScale() const { return _scale; }
            inline const Vec3f& getTranslation() const { return _translation; }
            inline const Mat4f& getLocalTransformation() const { return _localTransformation; }
            
            inline const Mat4f& getTransformation() const { return _transformation; }

            inline const bool isDirty() const { return _dirty; }
            
            // ------------------------------------------------------ SETTERS ------------------------------------------------------
            void setParent(SceneGraphNode* p_parent) {
                _parent = p_parent;
                update();
            }

            void setRotation(const Quatf& p_rotation) {
                _rotation = p_rotation;
                update();
            }

            void setScaling(const Vec3f& p_scale) {
                _scale = p_scale;
                update();
            }

            void setTranslation(const Vec3f& p_translation) {
                _translation = p_translation;
                update();
            }

            void setDirtyFalse() { _dirty = false; }

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------      
            void attach(SceneGraphNode* p_child) {
                _childs.push_back(p_child);
                p_child->setParent(this);
            }
            
            // detach ??
            void clearChilds() { _childs.clear(); }

            void update() {
                _dirty = true;
                _localTransformation = glm::translate(MAT4F_ID, _translation) * glm::mat4_cast(_rotation) * glm::scale(MAT4F_ID, _scale);
                _transformation = ((_parent==nullptr) ? MAT4F_ID : _parent->getTransformation()) * _localTransformation;
                for (SceneGraphNode* child : _childs) child->update();
            }

            void translate(const Vec3f& p_delta) {
                _translation += p_delta;
                update();
            }

            void rotate(const Vec3f& p_delta) { 
                _rotation *= Quatf(p_delta);
                update();
            }

            void scale(const Vec3f& p_delta) {
                _scale *= p_delta;
                update();
            }

            void rotateAround(const Vec3f& p_pivot, const Vec3f& p_delta) {
                Vec3f position = _transformation*Vec4f(VEC3F_ZERO,1.);
                translate(p_pivot + Quatf(p_delta) * (position - p_pivot) - position);
            }

            // todo implement
            void lookAt(const Vec3f& p_target) { }

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            SceneGraphNode* _parent     = nullptr;
            std::vector<SceneGraphNode*> _childs;

            Quatf _rotation             = QUATF_ID;
            Vec3f _scale                = VEC3F_ONE;
            Vec3f _translation          = VEC3F_ZERO;
            
            Mat4f _localTransformation  = MAT4F_ID;
            Mat4f _transformation       = MAT4F_ID;

            bool _dirty                 = false;
        };
    }
}

#endif
