require 'epeg'
require 'test/unit'

class TestEpeg < Test::Unit::TestCase
	def setup
		@epeg = Epeg.new('test.jpg')
	end
	def test_new
		assert_equal(Epeg, @epeg.class)
	end
	def test_size
		assert_equal(@epeg.size, [182, 170])
	end
end

class TestEpegSingleton < Test::Unit::TestCase
	def test_1
		assert(Epeg.thumbnail('test.jpg', 128, 128).size > 200)
	end
end
