using System;
using System.Runtime.CompilerServices;

namespace Nebula
{
    public class Transform
    {
        private uint entityID;

        internal Transform(uint id)
        {
            entityID = id;
        }

        public Vector3 position
        {
            get
            {
                InternalCalls.Entity_GetPosition(entityID, out Vector3 pos);
                return pos;
            }
            set
            {
                InternalCalls.Entity_SetPosition(entityID, ref value);
            }
        }

        public Vector3 rotation
        {
            get
            {
                InternalCalls.Entity_GetRotation(entityID, out Vector3 rot);
                return rot;
            }
            set
            {
                InternalCalls.Entity_SetRotation(entityID, ref value);
            }
        }

        public Vector3 scale
        {
            get
            {
                InternalCalls.Entity_GetScale(entityID, out Vector3 scl);
                return scl;
            }
            set
            {
                InternalCalls.Entity_SetScale(entityID, ref value);
            }
        }

        public Vector3 forward
        {
            get
            {
                // Calculate forward vector from rotation
                Quaternion q = Quaternion.Euler(rotation);
                return q * Vector3.Forward;
            }
        }

        public Vector3 right
        {
            get
            {
                Quaternion q = Quaternion.Euler(rotation);
                return q * Vector3.Right;
            }
        }

        public Vector3 up
        {
            get
            {
                Quaternion q = Quaternion.Euler(rotation);
                return q * Vector3.Up;
            }
        }

        // Hierarchy
        public Transform parent
        {
            get
            {
                uint parentID = InternalCalls.Entity_GetParent(entityID);
                if (parentID == 0)
                    return null;
                return new Transform(parentID);
            }
            set
            {
                uint parentID = value?.entityID ?? 0;
                InternalCalls.Entity_SetParent(entityID, parentID);
            }
        }

        public int childCount
        {
            get { return InternalCalls.Entity_GetChildCount(entityID); }
        }

        public Transform GetChild(int index)
        {
            uint childID = InternalCalls.Entity_GetChild(entityID, index);
            if (childID == 0)
                return null;
            return new Transform(childID);
        }

        public void SetParent(Transform parent)
        {
            this.parent = parent;
        }

        public void SetParent(Transform parent, bool worldPositionStays)
        {
            InternalCalls.Entity_SetParentWithTransform(entityID, parent?.entityID ?? 0, worldPositionStays);
        }
    }

    public class ScriptEntity
    {
        public uint ID { get; internal set; }

        private Transform _transform;
        public Transform transform
        {
            get
            {
                if (_transform == null)
                    _transform = new Transform(ID);
                return _transform;
            }
        }

        public string name
        {
            get { return InternalCalls.Entity_GetName(ID); }
            set { InternalCalls.Entity_SetName(ID, value); }
        }

        public string tag
        {
            get { return InternalCalls.Entity_GetTag(ID); }
            set { InternalCalls.Entity_SetTag(ID, value); }
        }

        public bool activeSelf
        {
            get { return InternalCalls.Entity_GetActiveSelf(ID); }
        }

        public bool activeInHierarchy
        {
            get { return InternalCalls.Entity_GetActiveInHierarchy(ID); }
        }

        public void SetActive(bool active)
        {
            InternalCalls.Entity_SetActive(ID, active);
        }

        internal ScriptEntity() { }

        // Unity-like component access methods
        public T GetComponent<T>() where T : class
        {
            Log.LogInfo($"GetComponent<{typeof(T).Name}> called on entity {ID} ('{name}')");
            
            // Handle Transform specially (returns wrapper)
            if (typeof(T) == typeof(Transform))
            {
                Log.LogInfo($"Returning Transform for entity {ID}");
                return transform as T;
            }

            // Handle ScriptBehavior subclasses (returns script instance)
            if (typeof(ScriptBehavior).IsAssignableFrom(typeof(T)))
            {
                if (!HasComponent<T>())
                {
                    Log.LogWarning($"Entity {ID} ('{name}') does not have script component {typeof(T).Name}");
                    return null;
                }

                object scriptInstance = InternalCalls.Entity_GetScriptInstance(ID, typeof(T));
                if (scriptInstance == null)
                {
                    Log.LogError($"Entity_GetScriptInstance returned null for {typeof(T).Name} on entity {ID}");
                }
                return scriptInstance as T;
            }

            // Handle regular data components
            if (!HasComponent<T>())
            {
                Log.LogWarning($"Entity {ID} ('{name}') does not have component {typeof(T).Name}");
                return null;
            }

            Log.LogInfo($"Creating instance of {typeof(T).Name} for entity {ID}");
            
            // Requires parameterless constructor for data components
            T component = System.Activator.CreateInstance<T>();
            if (component == null)
            {
                Log.LogError($"Failed to create instance of {typeof(T).Name} - Activator.CreateInstance returned null");
                return null;
            }
            
            // Set entity ID BEFORE getting component data for components that need it
            if (component is RigidBodyComponent rigidBody)
            {
                Log.LogInfo($"Setting RigidBodyComponent._entityID to {ID} BEFORE Entity_GetComponent");
                rigidBody._entityID = ID;
            }
            
            Log.LogInfo($"Calling Entity_GetComponent for {typeof(T).Name} on entity {ID}");
            if (!InternalCalls.Entity_GetComponent(ID, typeof(T), component))
            {
                Log.LogError($"Entity_GetComponent failed for {typeof(T).Name} on entity {ID} - component may not exist in C++");
                return null;
            }

            // Ensure entity ID is set AFTER getting component data (in case it was overwritten)
            if (component is RigidBodyComponent rigidBodyAfter)
            {
                Log.LogInfo($"Re-setting RigidBodyComponent._entityID to {ID} AFTER Entity_GetComponent");
                rigidBodyAfter._entityID = ID;
                Log.LogInfo($"Final RigidBodyComponent._entityID = {rigidBodyAfter._entityID}");
            }

            Log.LogInfo($"Successfully got component {typeof(T).Name} for entity {ID}");
            return component;
        }

        public bool HasComponent<T>() where T : class
        {
            return InternalCalls.Entity_HasComponent(ID, typeof(T));
        }

        public T AddComponent<T>() where T : class
        {
            if (HasComponent<T>())
            {
                Log.LogWarning($"Entity already has component of type {typeof(T).Name}");
                return GetComponent<T>();
            }

            InternalCalls.Entity_AddComponent(ID, typeof(T));
            return GetComponent<T>();
        }

        public void RemoveComponent<T>() where T : class
        {
            if (!HasComponent<T>())
            {
                Log.LogWarning($"Entity does not have component of type {typeof(T).Name}");
                return;
            }

            InternalCalls.Entity_RemoveComponent(ID, typeof(T));
        }

        // GetScript methods for accessing script instances
        public T GetScript<T>() where T : ScriptBehavior
        {
            return GetComponent<T>();
        }

        public ScriptBehavior GetScript(string className)
        {
            object scriptInstance = InternalCalls.Entity_GetScriptByName(ID, className);
            return scriptInstance as ScriptBehavior;
        }

        public T GetScriptByName<T>(string className) where T : ScriptBehavior
        {
            object scriptInstance = InternalCalls.Entity_GetScriptByName(ID, className);
            return scriptInstance as T;
        }

        public bool CompareTag(string tag)
        {
            return this.tag == tag;
        }

        // Legacy methods for backward compatibility
        public Vector3 GetPosition()
        {
            InternalCalls.Entity_GetPosition(ID, out Vector3 position);
            return position;
        }

        public void SetPosition(Vector3 position)
        {
            InternalCalls.Entity_SetPosition(ID, ref position);
        }

        public Vector3 GetRotation()
        {
            InternalCalls.Entity_GetRotation(ID, out Vector3 rotation);
            return rotation;
        }

        public void SetRotation(Vector3 rotation)
        {
            InternalCalls.Entity_SetRotation(ID, ref rotation);
        }

        public Vector3 GetScale()
        {
            InternalCalls.Entity_GetScale(ID, out Vector3 scale);
            return scale;
        }

        public void SetScale(Vector3 scale)
        {
            InternalCalls.Entity_SetScale(ID, ref scale);
        }
    }
}
