namespace Nebula
{
    public abstract class ScriptBehavior
    {
        public ScriptEntity Entity { get; internal set; }

        public Transform transform => Entity?.transform;

        private CoroutineManager _coroutineManager;
        protected CoroutineManager coroutineManager
        {
            get
            {
                if (_coroutineManager == null)
                    _coroutineManager = new CoroutineManager();
                return _coroutineManager;
            }
        }

        // Unity-like component access shortcuts
        public T GetComponent<T>() where T : class
        {
            return Entity?.GetComponent<T>();
        }

        public bool HasComponent<T>() where T : class
        {
            return Entity?.HasComponent<T>() ?? false;
        }

        public T AddComponent<T>() where T : class
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

        // Coroutine support
        public Coroutine StartCoroutine(System.Collections.IEnumerator routine)
        {
            return coroutineManager.StartCoroutine(routine);
        }

        public void StopCoroutine(Coroutine coroutine)
        {
            coroutineManager.StopCoroutine(coroutine);
        }

        // Unity-like lifecycle methods
        public virtual void OnCreate() { }
        public virtual void OnUpdate(float deltaTime)
        {
            // Update coroutines
            _coroutineManager?.Update();
        }
        public virtual void OnFixedUpdate() { }
        public virtual void LateUpdate() { }
        public virtual void OnEnable() { }
        public virtual void OnDisable() { }
        public virtual void OnDestroy() { }

        // Collision callbacks
        public virtual void OnCollisionEnter(Collision collision) { }
        public virtual void OnCollisionStay(Collision collision) { }
        public virtual void OnCollisionExit(Collision collision) { }

        // Trigger callbacks
        public virtual void OnTriggerEnter(ScriptEntity other) { }
        public virtual void OnTriggerStay(ScriptEntity other) { }
        public virtual void OnTriggerExit(ScriptEntity other) { }
    }
}
