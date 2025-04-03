extends Node2D

var object_name = "cooked-meal"

func _enter_tree() -> void:
	var holder = get_parent() as Person
	
	%PickUp.active = holder == null
	%Eat.active = holder != null
	if holder:
		%Eat.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		person.do_walk_to(global_position)
		person.do_grab(self)
		%PickUp.active = false
	elif action == %Eat:
		person.do_wait(4.0, 5.0)
		person.do_destroy()
		person.do_reward(action.advert)
		person.do_reward({"clean": -0.08})
