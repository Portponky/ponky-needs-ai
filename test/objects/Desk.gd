extends StaticBody2D

# A desk which people can use to generate paper. Each desk is for one person only.
# People can assign themselves to desks via tasks, and free the desk when they leave.

func plan(person: Person, action: Action) -> void:
	if action == %GetWorker:
		# A person wants to use this desk
		person.desk = self
		action.active = false
		
		# Set the tags for doing work for this person, and only when they are empty-handed
		%DoWork.set_tags(["empty-handed", person.full_name])
		%DoWork.active = true
		
		# Make them walk all the way up to the desk (to log on to the computer?)
		# This helps distribute the people after they arrive
		person.do_start_use(self)
		person.do_stop_use(self)
		person.do_reward(action.advert)
	
	elif action == %DoWork:
		# Attempting to get some work done
		person.do_start_use(self)
		person.do_wait(3.0, 5.0)
		
		# Get some paper
		person.do_gain("res://objects/Paper.tscn")
		person.do_stop_use(self)
		
		# No reward, the person has to do something with the paper to get that


func unassign() -> void:
	# The desk's person has left, so reset the actions
	%GetWorker.active = true
	%DoWork.active = false
