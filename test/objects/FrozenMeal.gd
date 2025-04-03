extends Node2D

var object_name = "frozen-meal"

func _enter_tree() -> void:
	var holder = get_parent() as Person
	%PickUp.active = holder == null


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		person.do_walk_to(global_position)
		person.do_grab(self)
		%PickUp.active = false
