enum State { FIRST, SECOND }

const MESSAGES: Dictionary[State, PackedStringArray] = {
	State.FIRST: ["alpha", "beta", "gamma"],
	State.SECOND: ["delta", "epsilon"],
}
const COUNTS: Dictionary[int, PackedInt32Array] = {0: [3, 5, 7]}
const ARRAYS: Array[PackedStringArray] = [["red", "green"], ["blue"]]
const FLOATS: Array[float] = [1, 2]
const NUMBERS: Dictionary[int, float] = {0: 3, 1: 4}

func test():
	var first: Variant = MESSAGES.get(State.FIRST)
	var second: Variant = MESSAGES.get(State.SECOND)
	var counts: Variant = COUNTS.get(0)
	var array: Array = ARRAYS.duplicate()
	var floats: Array = FLOATS.duplicate()
	var numbers: Dictionary = NUMBERS.duplicate()
	var key: State = State.FIRST

	assert(typeof(first) == TYPE_PACKED_STRING_ARRAY)
	assert(len(first) == 3)
	assert(first[0] == "alpha")
	assert(first[2] == "gamma")
	assert(second[1] == "epsilon")
	assert(typeof(counts) == TYPE_PACKED_INT32_ARRAY)
	assert(len(counts) == 3)
	assert(counts[1] == 5)
	assert(typeof(array[0]) == TYPE_PACKED_STRING_ARRAY)
	assert(len(array[0]) == 2)
	assert(array[0][1] == "green")
	assert(array[1][0] == "blue")
	assert(MESSAGES[key].size() == 3)
	assert(MESSAGES[key][0] == "alpha")
	assert(typeof(floats[0]) == TYPE_FLOAT)
	assert(floats[0] == 1.0)
	assert(typeof(numbers[0]) == TYPE_FLOAT)
	assert(numbers[0] == 3.0)
	print("Typed constant collections preserve their element types")
