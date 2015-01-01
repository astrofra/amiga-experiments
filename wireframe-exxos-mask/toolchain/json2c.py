import json

def main():
	vertex_list = []

	with open('../model/exxos_mask.json', 'rb') as fp:
		data = json.load(fp)

	# print(data)

	for _object in data:
		_object_dict = data[_object]['object']

		_object_name = _object_dict['object_name']
		print('Found an object, named \'%s\'' % _object_name)

		for _segment in _object_dict['segments']:
			for _vertex_as_array in _segment:
				print(_vertex_as_array)
				# _vertex = Vector3()

main()