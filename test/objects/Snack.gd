extends Node2D

# A bag of chips from a vending machine. Takes a bit to eat, and can be eaten
# while loitering around

# Takes three eating actions to finish the bag
var chomps := 3

func _enter_tree() -> void:
	# Are we being held?
	var holder = get_parent() as Person
	
	# Activate actions that are relevant
	%PickUp.active = holder == null
	%Chomp.active = holder != null
	if holder:
		# If being held, only the person holding it can eat
		%Chomp.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# This person is going to grab the snack from the floor
		person.do_walk_to(self)
		person.do_grab(self)
		%PickUp.active = false
	elif action == %Chomp:
		# Let's eat some chips
		person.do_loiter(1.2)
		
		# If this was the last chomp, get rid of the snack
		chomps -= 1
		if chomps == 0:
			person.do_destroy()
		
		# Reward the person
		person.do_reward(action.advert)
		# Snacks have side effects that the person didn't think about
		person.do_reward({"thirst": -0.05, "clean": -0.03})
