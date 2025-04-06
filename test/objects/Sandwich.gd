extends Node2D

func _enter_tree() -> void:
	var holder = get_parent() as Person
	
	%PickUp.active = holder == null
	%Eat.active = holder != null
	if holder:
		%Eat.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		person.do_walk_to(self)
		person.do_grab(self)
		person.do_reward({"social": 0.08})
		%PickUp.active = false
	elif action == %Eat:
		person.do_loiter(1.2)
		person.do_destroy()
		person.do_reward(action.advert)
