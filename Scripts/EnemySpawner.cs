using Nebula;
using System;

// Example demonstrating Unity-like prefab spawning system
public class EnemySpawner : ScriptBehavior
{
    private ScriptEntity enemyPrefab;
    private float spawnInterval = 2.0f;
    private float spawnTimer = 0.0f;
    private int maxEnemies = 10;

    public override void OnCreate()
    {
        Console.Log("EnemySpawner initialized!");
        
      =
        enemyPrefab = Find("EnemyTemplate");
        
        if (enemyPrefab == null)
        {
            Console.LogWarning("EnemyTemplate not found! Create an entity named 'EnemyTemplate' to use as a prefab.");
        }
    }

    public override void OnUpdate(float deltaTime)
    {
        if (enemyPrefab == null)
            return;

        spawnTimer += deltaTime;

        // Spawn enemies at intervals
        if (spawnTimer >= spawnInterval)
        {
            spawnTimer = 0.0f;

            // Check current enemy count
            ScriptEntity[] enemies = FindObjectsOfType<MeshRendererComponent>();
            
            if (enemies.Length < maxEnemies)
            {
                SpawnEnemy();
            }
        }
    }

    private void SpawnEnemy()
    {
        // Random spawn position around spawner
        float randomX = (float)(new Random().NextDouble() * 10.0 - 5.0);
        float randomZ = (float)(new Random().NextDouble() * 10.0 - 5.0);
        
        Vector3 spawnPosition = transform.position + new Vector3(randomX, 0, randomZ);
        Vector3 spawnRotation = new Vector3(0, (float)(new Random().NextDouble() * 360.0), 0);

        // Instantiate the prefab (Unity-like)
        ScriptEntity enemy = Instantiate(enemyPrefab, spawnPosition, spawnRotation);
        
        if (enemy != null)
        {
            // Customize the spawned enemy
            enemy.name = "Enemy_" + DateTime.Now.Ticks;
            Console.Log($"Spawned enemy at position {spawnPosition}");
        }
    }

    public override void OnDestroy()
    {
        Console.Log("EnemySpawner destroyed!");
    }
}
