extends Node2D

var chomps := 3

func _enter_tree() -> void:
	var holder = get_parent() as Person
	
	%PickUp.active = holder == null
	%Chomp.active = holder != null
	if holder:
		%Chomp.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		person.do_walk_to(global_position)
		person.do_grab(self)
		%PickUp.active = false
	elif action == %Chomp:
		person.do_loiter(1.2)
		chomps -= 1
		if chomps == 0:
			person.do_destroy()
		person.do_reward(action.advert)
		person.do_reward({"thirst": -0.05, "clean": -0.03}) # snacks have side effects
