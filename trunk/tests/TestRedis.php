<?php

require_once 'PHPUnit/Framework/TestCase.php';

class Redis_Test extends PHPUnit_Framework_TestCase
{
    /**
     * @var Redis
     */
    public $redis;

    public function setUp()
    {
        $this->redis = new Redis();
        $this->redis->connect('127.0.0.1', 6379);
    }
    
    public function tearDown()
    {
        $this->redis->close();
        unset($this->redis);
    }
    
    public function reset()
    {
        $this->setUp();
        $this->tearDown();
    }

    public function testPing()
    {
        $this->assertEquals('+PONG', $this->redis->ping());
    }
    
    public function testGet()
    {
        $this->redis->set('key', 'val');

        $this->assertEquals('val', $this->redis->get('key'));
    }

    public function testSet()
    {
        $this->redis->set('key', 'val1');

        $this->assertEquals('val1', $this->redis->get('key'));

        $this->redis->set('key', 'val2');

        $this->assertEquals('val2', $this->redis->get('key'));

        $this->redis->set('key', 42);

        $this->assertEquals('42', $this->redis->get('key'));
    }

    public function testAdd()
    {
        $key = 'key' . rand();

        $this->assertTrue($this->redis->add($key, 'val'));
        $this->assertFalse($this->redis->add($key, 'val'));
    }

    public function testIncr()
    {
        $this->redis->set('key', 0);

        $this->redis->incr('key');

        $this->assertEquals(1, $this->redis->get('key'));

        $this->redis->incr('key');

        $this->assertEquals(2, $this->redis->get('key'));

        $this->redis->incr('key', 3);

        $this->assertEquals(5, $this->redis->get('key'));
    }

    public function testDecr()
    {
        $this->redis->set('key', 5);

        $this->redis->decr('key');

        $this->assertEquals(4, $this->redis->get('key'));

        $this->redis->decr('key');

        $this->assertEquals(3, $this->redis->get('key'));

        $this->redis->decr('key', 2);

        $this->assertEquals(1, $this->redis->get('key'));
    }

    public function testExists()
    {
        $key = 'key' . rand();

        $this->assertFalse($this->redis->exists($key));

        $this->redis->add($key, 'val');

        $this->assertTrue($this->redis->exists($key));
    }

    public function testGetKeys()
    {
        $keys = $this->redis->getKeys('a*');
        $key  = 'a' . rand();

        $this->redis->add($key, 'val');

        $keys2 = $this->redis->getKeys('a*');

        $this->assertEquals((count($keys) + 1), count($keys2));
    }

    public function testDelete()
    {
        $this->redis->set('key', 'val');

        $this->assertEquals('val', $this->redis->get('key'));

        $this->redis->delete('key');

        $this->assertEquals(null, $this->redis->get('key'));
    }

    public function testListPop()
    {
        $this->redis->delete('list');

        $this->redis->listPush('list', 'val');
        $this->redis->listPush('list', 'val2');
        $this->redis->listPush('list', 'val3', 1);

        $this->assertEquals('val3', $this->redis->listPop('list', 1));
        $this->assertEquals('val2', $this->redis->listPop('list'));
        $this->assertEquals('val', $this->redis->listPop('list'));
    }

    public function testListSize()
    {
        $this->redis->delete('list');

        $this->redis->listPush('list', 'val');
        
        $this->assertEquals(1, $this->redis->listSize('list'));

        $this->redis->listPush('list', 'val');
        
        $this->assertEquals(2, $this->redis->listSize('list'));
    }
}
?>
