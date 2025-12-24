namespace Nebula
{
    public abstract class ScriptBehavior
    {
        public ScriptEntity Entity { get; internal set; }

        public Transform transform => Entity?.transform;

        public virtual void OnCreate() { }
        public virtual void OnUpdate(float deltaTime) { }
        public virtual void OnDestroy() { }
    }
}
