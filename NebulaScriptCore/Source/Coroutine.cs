using System;
using System.Collections;
using System.Collections.Generic;

namespace Nebula
{
    // Base class for yield instructions
    public abstract class YieldInstruction
    {
        internal abstract bool KeepWaiting { get; }
    }

    // Wait for given seconds (simulate with real time)
    public class WaitForSeconds : YieldInstruction
    {
        private readonly float _seconds;
        private readonly DateTime _startTime;

        public WaitForSeconds(float seconds)
        {
            _seconds = seconds;
            _startTime = DateTime.UtcNow;
        }

        internal override bool KeepWaiting => (DateTime.UtcNow - _startTime).TotalSeconds < _seconds;
    }

    // Wait until a condition is true
    public class WaitUntil : YieldInstruction
    {
        private readonly Func<bool> _predicate;

        public WaitUntil(Func<bool> predicate)
        {
            _predicate = predicate ?? throw new ArgumentNullException(nameof(predicate));
        }

        internal override bool KeepWaiting => !_predicate();
    }

    // The Coroutine class wraps IEnumerator and current yield instruction
    public class Coroutine
    {
        private readonly IEnumerator _routine;
        private YieldInstruction _currentYield;

        public bool IsComplete { get; private set; }

        public Coroutine(IEnumerator routine)
        {
            _routine = routine ?? throw new ArgumentNullException(nameof(routine));
        }

        // Advances coroutine if it can
        public void Update()
        {
            if (IsComplete)
                return;

            if (_currentYield != null && _currentYield.KeepWaiting)
                return;

            if (!_routine.MoveNext())
            {
                IsComplete = true;
                return;
            }

            _currentYield = _routine.Current as YieldInstruction;
            // If yielded null or no YieldInstruction, _currentYield will be null and coroutine continues next Update()
        }
    }

    // The CoroutineManager handles all active coroutines
    public class CoroutineManager
    {
        private readonly List<Coroutine> _coroutines = new List<Coroutine>();

        public Coroutine StartCoroutine(IEnumerator routine)
        {
            var coroutine = new Coroutine(routine);
            _coroutines.Add(coroutine);
            return coroutine;
        }

        public void StopCoroutine(Coroutine coroutine)
        {
            _coroutines.Remove(coroutine);
        }

        public void Update()
        {
            for (int i = _coroutines.Count - 1; i >= 0; i--)
            {
                var coroutine = _coroutines[i];
                coroutine.Update();
                if (coroutine.IsComplete)
                    _coroutines.RemoveAt(i);
            }
        }
    }
}
