extends Node2D

func _enter_tree() -> void:
	var held = get_parent() is Person
	%PickUp.active = !held


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		person.do_walk_to(global_position)
		person.do_grab(self)
		%PickUp.active = false
