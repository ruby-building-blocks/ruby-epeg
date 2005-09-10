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
	def test_finish
		assert(@epeg.finish)
		assert_raises(StandardError) { @epeg.finish }
	end
end

class TestEpegSingleton < Test::Unit::TestCase
	def test_1
		assert(Epeg.thumbnail('test.jpg', 128, 128).size > 200)
	end
end
