namespace Nebula
{
    public abstract class ScriptBehavior
    {
        public ScriptEntity Entity { get; internal set; }

        public Transform transform => Entity?.transform;

        // Unity-like component access shortcuts
        public T GetComponent<T>() where T : class, new()
        {
            return Entity?.GetComponent<T>();
        }

        public bool HasComponent<T>() where T : class
        {
            return Entity?.HasComponent<T>() ?? false;
        }

        public T AddComponent<T>() where T : class, new()
        {
            return Entity?.AddComponent<T>();
        }

        public void RemoveComponent<T>() where T : class
        {
            Entity?.RemoveComponent<T>();
        }

        // Unity-like scene query methods
        public static ScriptEntity[] FindObjectsOfType<T>() where T : class
        {
            return GameObject.FindObjectsOfType<T>();
        }

        public static ScriptEntity FindObjectOfType<T>() where T : class
        {
            return GameObject.FindObjectOfType<T>();
        }

        public static ScriptEntity Find(string name)
        {
            return GameObject.Find(name);
        }

        public static ScriptEntity Instantiate(ScriptEntity prefab)
        {
            return GameObject.Instantiate(prefab);
        }

        public static ScriptEntity Instantiate(ScriptEntity prefab, Vector3 position, Vector3 rotation)
        {
            return GameObject.Instantiate(prefab, position, rotation);
        }

        public static void Destroy(ScriptEntity entity)
        {
            GameObject.Destroy(entity);
        }

        public static void Destroy(ScriptEntity entity, float delay)
        {
            GameObject.Destroy(entity, delay);
        }

        public virtual void OnCreate() { }
        public virtual void OnUpdate(float deltaTime) { }
        public virtual void OnDestroy() { }
    }
}
