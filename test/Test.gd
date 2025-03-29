extends Node2D

func _ready() -> void:
	var person_count = 0
	for node in $Things.get_children():
		if node is not Person:
			continue
		
		person_count += 1
		if person_count > 1:
			node.queue_free()
