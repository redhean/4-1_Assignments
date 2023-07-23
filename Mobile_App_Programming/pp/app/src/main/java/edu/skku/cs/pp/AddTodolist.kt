package edu.skku.cs.pp

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.CheckBox
import android.widget.EditText
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.IOException

class AddTodolist : AppCompatActivity() {

    data class Calendar(
        val name: String,
        val events: List<Event>
    )

    data class Event(
        val date: String,
        val title: String,
        val description: String,
        val check: Boolean
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_add_todolist)

        supportActionBar?.setTitle("DayFlow")

        val calendarTitle = intent.getStringExtra(MainActivity.EXT_NAME)
        val selectedDate = intent.getStringExtra(MainActivity.EXT_DATE)
        val chk = intent.getStringExtra(MainActivity.EXT_CHK)
        val beforeTitle = intent.getStringExtra("beforeTitle")
        val beforeDesc = intent.getStringExtra("beforeDesc")

        val newTodoTitle = findViewById<EditText>(R.id.newToDoTitleInput)
        val newTodoDesc = findViewById<EditText>(R.id.newToDoDescInput)

        val submitBtn = findViewById<Button>(R.id.submit2)

        val client = OkHttpClient()

        submitBtn.setOnClickListener {
            val currCalendarTitle = calendarTitle
            val eventTitle = newTodoTitle.text.toString()
            val eventDescription = newTodoDesc.text.toString()
            val eventDate = selectedDate

            val jsonEventData = """
                {
                    "calendar": "$currCalendarTitle",
                    "date": "$eventDate",
                    "title": "$eventTitle",
                    "description": "$eventDescription",
                    "beforeTitle" : "$beforeTitle",
                    "beforeDesc" : "$beforeDesc",
                    "check": false
                    
                }
                """.trimIndent()

            val url = "url_link"
            val requestBody = jsonEventData.toRequestBody("application/json".toMediaTypeOrNull())
            val request: Request

            if (chk == "no") {
                request = Request.Builder().url(url).put(requestBody).build()
                Log.i("test", chk.toString())
            } else {
                request = Request.Builder().url(url).post(requestBody).build()
            }

            client.newCall(request).enqueue(object : okhttp3.Callback {
                override fun onFailure(call: okhttp3.Call, e: IOException) {
                    e.printStackTrace()
                }

                override fun onResponse(call: okhttp3.Call, response: okhttp3.Response) {
                    if (!response.isSuccessful) throw IOException("Unexpected code $response")
                    CoroutineScope(Dispatchers.Main).launch{

                        finish()
                    }
                }
            })
        }
    }
}