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

    public function testListTrim()
    {
        $this->redis->delete('list');

        $this->redis->listPush('list', 'val');
        $this->redis->listPush('list', 'val2');
        $this->redis->listPush('list', 'val3');

        $this->redis->listTrim('list', 0, 0);
  
        $this->assertEquals(1, $this->redis->listSize('list'));
        $this->assertEquals('val', $this->redis->listPop('list'));
    }

    public function testListGet()
    {
        $this->redis->delete('list');

        $this->redis->listPush('list', 'val');
        $this->redis->listPush('list', 'val2');
        $this->redis->listPush('list', 'val3');

        $this->assertEquals('val', $this->redis->listGet('list', 0));
        $this->assertEquals('val2', $this->redis->listGet('list', 1));
        $this->assertEquals('val3', $this->redis->listGet('list', 2));

        $this->redis->listPush('list', 'val4');
        $this->assertEquals('val4', $this->redis->listGet('list', 3));
    }

    public function testSetAdd()
    {
        $this->redis->delete('set');

        $this->redis->setAdd('set', 'val');

        $this->assertTrue($this->redis->setContains('set', 'val'));
        $this->assertFalse($this->redis->setContains('set', 'val2'));

        $this->redis->setAdd('set', 'val2');

        $this->assertTrue($this->redis->setContains('set', 'val2'));
    }

    public function testSetSize()
    {
        $this->redis->delete('set');

        $this->redis->setAdd('set', 'val');
        
        $this->assertEquals(1, $this->redis->setSize('set'));

        $this->redis->setAdd('set', 'val2');
        
        $this->assertEquals(2, $this->redis->setSize('set'));
    }

    public function testSetRemove()
    {
        $this->redis->delete('set');

        $this->redis->setAdd('set', 'val');
        $this->redis->setAdd('set', 'val2');
        
        $this->redis->setRemove('set', 'val');

        $this->assertEquals(1, $this->redis->setSize('set'));

        $this->redis->setRemove('set', 'val2');

        $this->assertEquals(0, $this->redis->setSize('set'));
    }

    public function testSetContains()
    {
        $this->redis->delete('set');

        $this->redis->setAdd('set', 'val');
        
        $this->assertTrue($this->redis->setContains('set', 'val'));
        $this->assertFalse($this->redis->setContains('set', 'val2'));
    }

    public function testSetGetMembers()
    {
        $this->redis->delete('set');

        $this->redis->setAdd('set', 'val');
        $this->redis->setAdd('set', 'val2');
        $this->redis->setAdd('set', 'val3');

        $array = array('val', 'val2', 'val3');
        
        $this->assertEquals($array, $this->redis->setGetMembers('set'));
    }
}

?>