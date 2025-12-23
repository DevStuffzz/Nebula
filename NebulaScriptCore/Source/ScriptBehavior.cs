namespace Nebula
{
    public abstract class ScriptBehavior
    {
        public ScriptEntity Entity { get; internal set; }

        public virtual void OnCreate() { }
        public virtual void OnUpdate(float deltaTime) { }
        public virtual void OnDestroy() { }
    }
}
