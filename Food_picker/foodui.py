"""
Ever been unable to choose what to make for dinner? Now there's a solution!
Food machine ie. a program to randomly choose a recipe from a recipe database.
The database includes three tables, one to store recipes and includes cells 
'id' primary key integer, 'name' text, 'ingredients' text and 'method' text.
One stores recipe categories including cells 'id' primary key integer and
'type' text and the last table contains recipes according to their types 
including cells 'id' primary key integer, 'recipe_id' integer and 'type_name'
text.

The GUI is implemented with tkinter and is only there to make the userinterface
easier to use. The main focus of the program is making everyday decisions 
easier and to learn to use SQL databases. The database is currently not 
publicly available but just is implemented in PosgreSQL environment.

Contributor: Essi Pietilä @epietila
"""


import psycopg2
from tkinter import *

# Connect to database
conn = psycopg2.connect(
    "host= localhost dbname=recipedb user=postgres password=uusSalasana")
# Cursor to perform operations
cur = conn.cursor()

category_table_name = "recipedbschema.recipeTypes"
recipe_table_name = "recipedbschema.recipes"

BUTTON_COLOUR = "#42f58d"


class Userinterface:
    def __init__(self):
        self.__window = Tk()
        self.__window.title("Food machine")
        self.__window.configure(background='white')
        self.__window.columnconfigure([0, 1, 2, 3], weight=1, minsize=50)
        self.__window.rowconfigure([0, 1, 2, 3], weight=1, minsize=50)

        # Cats stores category names as a list
        self.__cats = []

        # Save available recipe types to cats
        cur.execute("select type_name from " + category_table_name + ";")
        cats_list = cur.fetchall()
        for i in cats_list:
            self.__cats.append(i[0])

        self.__cat_buttons = []
        self.__rec_id = 0
        self.__rec_name = ""
        self.__first_rec = True

        self.__welcome_label = Label(self.__window,
                                     text="Welcome to recipe finder! \n Get "
                                          "your food of the day by clicking the"
                                          " category of your preference.",
                                     bg="white")
        self.__welcome_label.grid(row=0, columnspan=3, sticky=NS)
        self.__empty_label = Label(self.__window, bg="white")

        self.create_buttons()

    # Create buttons to choose the category
    def create_buttons(self):
        i = 0
        j = 0
        # Create the buttons and append to cat_buttons
        for k in range(len(self.__cats)):
            new_button = Button(self.__window, text=self.__cats[k],
                                bg=BUTTON_COLOUR, width=10)
            new_button.grid(row=i + 1, column=j, padx=2, pady=2, sticky=N)
            new_button.configure(
                command=(lambda x=self.__cats[k]: self.get_recipe(x)))
            self.__cat_buttons.append(new_button)

            j += 1
            if j == 2:
                j = 0
                i += 1

    # Get id to a recipe of the clicked category
    def get_recipe(self, cat_name):
        cur.execute(
            "select recipe_id from recipedbschema.recipeByType where "
            "type_name='%s' order by type_name, random() limit 1;" % cat_name)
        self.__rec_id = int(cur.fetchall()[0][0])
        cur.execute(
            "select name from " + recipe_table_name + " where id=%d ;"
            % self.__rec_id)
        self.__rec_name = cur.fetchall()[0][0]
        self.recipe_window()

    # Get ingredients for the chosen recipe
    def ingredients(self):
        cur.execute(
            "select ingredients from " + recipe_table_name + " where id=%d;"
            % self.__rec_id)
        self.format_output(cur.fetchall()[0][0])

    # Fetch the method of the chosen recipe
    def method(self):
        cur.execute(
            "select method from " + recipe_table_name + " where id=%d;"
            % self.__rec_id)
        self.format_output(cur.fetchall()[0][0])

    # Print information about the recipe in the text object
    def format_output(self, string):
        self.__recipeinfo.delete(1.0, END)
        s = string.split("\n")
        for ln in s:
            self.__recipeinfo.insert(END, ln + "\n")

    # Return to the window for choosing a recipe category
    def new_recipe(self):
        # Hide recipe handling items
        self.__info_label.grid_forget()
        self.__recipe_label.grid_forget()
        self.__ingredient_button.grid_forget()
        self.__method_button.grid_forget()
        self.__other_button.grid_forget()
        self.__recipeinfo.delete(1.0, END)
        self.__recipeinfo.grid_forget()

        # Set welcome label and category buttons visible
        self.__welcome_label.grid()
        i = 0
        j = 0
        for b in self.__cat_buttons:
            b.grid(row=i + 2, column=j, padx=2, pady=2)
            j += 1
            if j == 2:
                j = 0
                i += 1

    # Create the object required to view the recipe
    def recipe_stuff(self):
        self.__info_label = Label(self.__window,
                                  text="Your recipe of the day is:",
                                  bg="white")
        self.__recipe_label = Label(self.__window, text=self.__rec_name,
                                    bg="white")
        self.__recipeinfo = Text(self.__window, bg="white", width=80)

        self.__ingredient_button = Button(self.__window, text="Ingredients",
                                          bg=BUTTON_COLOUR, width=10)
        self.__ingredient_button.configure(command=self.ingredients)

        self.__method_button = Button(self.__window, text="Method",
                                      bg=BUTTON_COLOUR, width=10)
        self.__method_button.configure(command=self.method)

        self.__other_button = Button(self.__window, text="New recipe",
                                     bg=BUTTON_COLOUR, width=10)
        self.__other_button.configure(command=self.new_recipe)

    # Create the window for viewing the recipe
    def recipe_window(self):
        # Hide unnecessary items
        self.__welcome_label.grid_forget()
        for b in self.__cat_buttons:
            b.grid_forget()

        # If this is the first recipe of the run, create new items
        if self.__first_rec:
            self.recipe_stuff()
            self.__first_rec = False
        else:
            self.__recipe_label.configure(text=self.__rec_name)

        # Set items visible
        self.__info_label.grid(row=1, columnspan=4)
        self.__recipe_label.grid(row=2, columnspan=4)
        self.__recipeinfo.grid(row=6, rowspan=6, column=1, columnspan=6)

        self.__ingredient_button.grid(row=4, column=1, padx=2, pady=2)
        self.__method_button.grid(row=4, column=2, padx=2, pady=2)
        self.__other_button.grid(row=4, column=3, padx=2, pady=2)

    # Start the mainloop
    def start(self):
        self.__window.mainloop()


def main():
    ui = Userinterface()
    ui.start()


main()