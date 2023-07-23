package edu.skku.cs.pa1

import android.content.Context
import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import android.util.Log
import android.view.inputmethod.InputMethodManager
import com.example.eduskkucspa1.R
import java.io.InputStream
import java.util.Random

class MainActivity : AppCompatActivity() {
    class Wordle(var ans : String, var submit : String)

    class Letter(var char : String, var color : Int) {
        override fun equals(other: Any?): Boolean {
            return if(other is Letter) {
                this.char == other.char
            } else {
                false
            }
        }

        override fun hashCode(): Int {
            var result = char.hashCode()
            result = 31 * result + color
            return result
        }
    }

    class CustomAdapter_word(val context: Context, val items : List<Wordle>) : BaseAdapter(){
        override fun getCount(): Int {
            return items.size
        }

        override fun getItem(p0: Int): Any {
            return items.get(p0)
        }

        override fun getItemId(p0: Int): Long {
            return 0
        }

        override fun getView(p0: Int, p1: View?, p2: ViewGroup?): View {
            val inflater:LayoutInflater = LayoutInflater.from(context)
            val view : View = inflater.inflate(R.layout.word_list,null)

            var textView1 = view.findViewById<TextView>(R.id.textView1)
            var textView2 = view.findViewById<TextView>(R.id.textView2)
            var textView3 = view.findViewById<TextView>(R.id.textView3)
            var textView4 = view.findViewById<TextView>(R.id.textView4)
            var textView5 = view.findViewById<TextView>(R.id.textView5)

            /* color == 1 -> Right Letter & Right position
            *  color == 2 -> Right Letter & Wrong position
            *  color == 3 -> Wrong Letter
            * */

            val color = arrayListOf<Int>()

            for(i in items.get(p0).ans.indices){
                if(items.get(p0).ans[i] == items.get(p0).submit[i]){
                    color.add(1)
                } else if(items.get(p0).ans.contains(items.get(p0).submit[i])){
                    color.add(2)
                }
                else{
                    color.add(3)
                }

            }

            /* To uppercase */

            textView1.setText(items.get(p0).submit[0].toString().toUpperCase())
            if(color[0] == 1){
                textView1.setBackgroundColor(Color.parseColor("#FF99F691"))
                textView1.setTextColor(Color.parseColor("#FF000000"))
            }
            else if(color[0] == 2){
                textView1.setBackgroundColor(Color.parseColor("#FFFFE46F"))
                textView1.setTextColor(Color.parseColor("#FF000000"))
            }
            else{
                textView1.setBackgroundColor(Color.parseColor("#FF787C7E"))
                textView1.setTextColor(Color.parseColor("#FFFFFFFF"))
            }

            textView2.setText(items.get(p0).submit[1].toString().toUpperCase())
            if(color[1] == 1){
                textView2.setBackgroundColor(Color.parseColor("#FF99F691"))
                textView2.setTextColor(Color.parseColor("#FF000000"))
            }
            else if(color[1] == 2){
                textView2.setBackgroundColor(Color.parseColor("#FFFFE46F"))
                textView2.setTextColor(Color.parseColor("#FF000000"))
            }
            else{
                textView2.setBackgroundColor(Color.parseColor("#FF787C7E"))
                textView2.setTextColor(Color.parseColor("#FFFFFFFF"))
            }

            textView3.setText(items.get(p0).submit[2].toString().toUpperCase())
            if(color[2] == 1){
                textView3.setBackgroundColor(Color.parseColor("#FF99F691"))
                textView3.setTextColor(Color.parseColor("#FF000000"))
            }
            else if(color[2] == 2){
                textView3.setBackgroundColor(Color.parseColor("#FFFFE46F"))
                textView3.setTextColor(Color.parseColor("#FF000000"))
            }
            else{
                textView3.setBackgroundColor(Color.parseColor("#FF787C7E"))
                textView3.setTextColor(Color.parseColor("#FFFFFFFF"))
            }


            textView4.setText(items.get(p0).submit[3].toString().toUpperCase())
            if(color[3] == 1){
                textView4.setBackgroundColor(Color.parseColor("#FF99F691"))
                textView4.setTextColor(Color.parseColor("#FF000000"))
            }
            else if(color[3] == 2){
                textView4.setBackgroundColor(Color.parseColor("#FFFFE46F"))
                textView4.setTextColor(Color.parseColor("#FF000000"))
            }
            else{
                textView4.setBackgroundColor(Color.parseColor("#FF787C7E"))
                textView4.setTextColor(Color.parseColor("#FFFFFFFF"))
            }

            textView5.setText(items.get(p0).submit[4].toString().toUpperCase())
            if(color[4] == 1){
                textView5.setBackgroundColor(Color.parseColor("#FF99F691"))
                textView5.setTextColor(Color.parseColor("#FF000000"))
            }
            else if(color[4] == 2){
                textView5.setBackgroundColor(Color.parseColor("#FFFFE46F"))
                textView5.setTextColor(Color.parseColor("#FF000000"))
            }
            else {
                textView5.setBackgroundColor(Color.parseColor("#FF787C7E"))
                textView5.setTextColor(Color.parseColor("#FFFFFFFF"))
            }

            return view
        }

    }

    class CustomAdapter_letter(val context : Context, val items : List<Letter>) : BaseAdapter(){
        override fun getCount(): Int {
            return items.size
        }

        override fun getItem(p0: Int): Any {
            return items.get(p0)
        }

        override fun getItemId(p0: Int): Long {
            return 0
        }

        override fun getView(p0: Int, p1: View?, p2: ViewGroup?): View {
            val inflater:LayoutInflater = LayoutInflater.from(context)
            val view : View = inflater.inflate(R.layout.letter_list,null)

            var textView = view.findViewById<TextView>(R.id.textView6)
            var input = items.get(p0).char.toString()
            var clr = items.get(p0).color

            textView.setText(input.toUpperCase())

            if(clr == 1){
                textView.setBackgroundColor(Color.parseColor("#FF99F691"))
                textView.setTextColor(Color.parseColor("#FF000000"))
            }
            else if(clr == 2){
                textView.setBackgroundColor(Color.parseColor("#FFFFE46F"))
                textView.setTextColor(Color.parseColor("#FF000000"))
            }
            else{
                textView.setBackgroundColor(Color.parseColor("#FF787C7E"))
                textView.setTextColor(Color.parseColor("#FFFFFFFF"))
            }

            return view
        }
    }

    /* "\n"하고 "\r" 둘 다 고려 해줘야 햠*/
    private val wordleList by lazy {
        this.assets.open("wordle_words.txt")
            .bufferedReader()
            .use {
                it.readText()
            }
            .replace("\r", "")
            .split("\n")
            .filter {
                it.isNotBlank()
            }
    }

    /* random answer */
    private val ans by lazy { wordleList.random() }

    private val btnsubmit by lazy { findViewById<Button>(R.id.btnSubmit) }
    private var words = arrayListOf<Wordle>()
    /* 중복 제어를 위해 set 으로 설정 */
    private var letter_left = mutableSetOf<Letter>()
    private var letter_middle = mutableSetOf<Letter>()
    private var letter_right = mutableSetOf<Letter>()
    private val input_submit by lazy { findViewById<EditText>(R.id.editTextInput) }

    private val imm: InputMethodManager by lazy {
        getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btnsubmit.setOnClickListener {
            onClickBtnSubmit()
            imm.hideSoftInputFromWindow(input_submit.windowToken, 0)
        }
    }

    private fun onClickBtnSubmit() {
        val input = input_submit.text.toString().toLowerCase()

        // 단어가 없을때
        if(!wordleList.contains(input)){
            Toast.makeText(
                applicationContext,
                "Word '${input}' not in dictionary!",
                Toast.LENGTH_SHORT
            ).show()
        }
        // 단어가 있을 때
        else{
            input_submit.text.clear()
            val color = arrayListOf<Int>()
            val wordle = Wordle(ans, input)
            words.add(wordle)

            for(i in ans.indices){
                if(ans[i] == input[i]){
                    color.add(1)
                } else if(ans.contains(input[i])) {
                    color.add(2)
                }
                else{
                    color.add(3)
                }
            }

            /* 중복 제거 필요 */
            for(i in color.indices){
                if(color[i] == 1){
                    val char = input.substring(i, i +1)
                    letter_left = letter_left.filter { it.char != char }.toMutableSet()
                    letter_middle = letter_middle.filter { it.char != char }.toMutableSet()
                    val letter = Letter(char, 1)
                    letter_right.add(letter)
                    continue
                }
                else if(color[i] == 2){
                    val char = input.substring(i, i +1)
                    letter_left = letter_left.filter { it.char != char }.toMutableSet()
                    val letter = Letter(char, 2)
                    if(!letter_right.map { it.char }.contains(char)) {
                        letter_middle.add(letter)
                    }
                    continue

                }
                else if(color[i] == 3) {
                    val char = input.substring(i, i +1)
                    val letter = Letter(char, 3)
                    if(!letter_right.map{it.char}.contains(char) && !letter_middle.map{it.char}.contains(char)){
                        letter_left.add(letter)
                    }
                }
            }

            val listAdapter = CustomAdapter_word(this, words)
            val mainList = findViewById<ListView>(R.id.wordList)

            val letter_left_Adapter = CustomAdapter_letter(this, letter_left.toList())
            val letter_middle_Adapter = CustomAdapter_letter(this, letter_middle.toList())
            val letter_right_Adapter = CustomAdapter_letter(this, letter_right.toList())

            val letter_left_List = findViewById<ListView>(R.id.letterList1)
            val letter_middle_List = findViewById<ListView>(R.id.letterList2)
            val letter_right_List = findViewById<ListView>(R.id.letterList3)

            letter_left_List.adapter = letter_left_Adapter
            letter_middle_List.adapter = letter_middle_Adapter
            letter_right_List.adapter = letter_right_Adapter
            mainList.adapter = listAdapter
        }
    }
}