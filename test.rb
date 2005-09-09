require 'epeg'
require 'test/unit'

class TestEpeg < Test::Unit::TestCase
	def setup
		@epeg = Epeg.thumbnail('test.jpg', 128, 128)
	end
	def test_1
		assert(@epeg.size > 200)
	end
end
